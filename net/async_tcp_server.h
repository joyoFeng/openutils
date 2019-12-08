#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <thread>


using namespace std;


class async_tcp_server
{
private:
    string _host;
    int _port;
    volatile bool _running = false;
    int _fd = -1;
    struct sockaddr_in _addr;

    fd_set _readset;
    struct timeval _interval;
    int _error = 0;
    thread* _server_thread = nullptr;


public:
    //create async, re-useable, no-delay socket, bind host:port, listen
    async_tcp_server(const string &host, short port, bool resuable = true, bool nodelay = true)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);

        int enable = 1;
        if (resuable)
        {
            //RE-USEABLE
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_NOSIGPIPE, &enable, sizeof(enable)) < 0)
            {
                fprintf(stderr, "SO_REUSEADDR|SO_NOSIGPIPE set failed, errno: %d\n", errno);
                _error = errno;
                return;
            }
        }

        //SOL_TCP
        if (nodelay)
        {
            if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0)
            {
                fprintf(stderr, "TCP_NODELAY set failed, errno: %d\n", errno);
                _error = errno;
                return;
            }
        }

        //bind
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.data());
        addr.sin_port = htons(port);
        if (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            fprintf(stderr, "bind to %s:%d failed, errno: %d\n", host.data(), (int)port, errno);
            _error = errno;
            return;
        }

        //listen
        int backlog = 100;
        if (listen(fd, backlog) < 0)
        {
            fprintf(stderr, "bind to %s:%d failed\n", host.data(), (int)port);
            _error = errno;
            return;
        }

        _fd = fd;
        //set non-blocking
        set_nonblocking(_fd);
        
        _host = host;
        _port = port;
        _running = false;
        _addr = addr;
        _error = 0;
        FD_ZERO(&_readset);
        _interval.tv_sec = 0;
        _interval.tv_usec = 10000;//10ms

        printf("create async server socket success.\n");
    }

    ~async_tcp_server()
    {
        stop();
    }

    int set_nonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        return fcntl(fd, F_SETFL, new_option);
    }

    // stop serving and close sockets
    void stop()
    {
        if (!_running)
            return;

        {
            _running = false;
            _server_thread->join();
            delete _server_thread;
            _server_thread = nullptr;

            close(_fd);
            _fd = -1;
        }
        
        printf("closed server.\n");
    }

    void waitforshutdown()
    {
        if (_running)
        {
            _server_thread->join();
            delete _server_thread;
            _server_thread = nullptr;
            _running = false;
            close(_fd);
            _fd = -1;
        }

    }

    void start()
    {
        if (_running)
        {
            printf("server already started.\n");
            return;
        }

        _running = true;
        _server_thread = new thread([&](async_tcp_server* serv){serv->serving();}, this);
        printf("start serving thread...\n");
    }

private:
    static void run_routine(void *server)
    {
        async_tcp_server *serv = (async_tcp_server *)server;
        serv->serving(); //select polling
    }

    // running thread to run async-accept or recv message from read_fd_set
    void serving()
    {
        printf("****  serving...\n");
        FD_SET(_fd, &_readset); //add server fd
        fd_set testfds;

        while (_running)
        {
            //copy readset
            FD_COPY(&_readset, &testfds);

            //do select polliing
            int result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &_interval); //FD_SETSIZE：系统默认的最大文件描述符
            if (result < 0)//return 0 when expire
            {
                perror("select _error");
                _error = errno;
                close(_fd);
                break;
            }
            else if (result == 0)
            {
                //printf("timeout...\n");
                continue;
            }

            /* scan fd */
            for (int fd = 0; fd < FD_SETSIZE; fd++)
            {
                /* check if readable */
                if (FD_ISSET(fd, &testfds))
                {
                    //maybe we should check if the fd is valid, with SO_SOCKET, SO_TYPE
                    /* server fd readable, get new connect */
                    if (fd == _fd)
                    {
                        struct sockaddr client_address;
                        socklen_t client_len = sizeof(client_address);
                        int client_sockfd = accept(_fd, (struct sockaddr*)&client_address, &client_len);
                        FD_SET(client_sockfd, &_readset); //add to listen readfdsets
                        printf("adding client on fd %d\n", client_sockfd);
                    }
                    /* recv message from clients */
                    else
                    {
                        //1. recv msg size
                        int msg_size = 0;
                        int bytes = 0;
                        int len = sizeof(msg_size);
                        
                        bool fd_closed = false;
                        int so_errno = 0;
                        socklen_t so_errno_len = sizeof(so_errno);
                        while (bytes < len)
                        {
                            int ret = recv(fd, ((char*)(&msg_size))+bytes, sizeof(msg_size)-bytes, 0);
                            getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_errno, &so_errno_len);
                            if (ret == 0 || ret < 0 && so_errno != 0 && so_errno != EAGAIN && so_errno != EWOULDBLOCK && so_errno != ETIMEDOUT)
                            //if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != ETIMEDOUT)
                            {
                                FD_CLR(fd, &_readset);
                                ::close(fd);
                                printf("ret: %d, errno: %d, closing the connect fd:%d\n", ret, errno, fd);
                                fd_closed = true;
                                break;
                            }
                            // else if (ret == 0)
                            // {
                            //     printf("warn: client close the connection. errno: %d, fd: %d\n", errno, fd);

                            // }    

                            bytes += ret;
                            usleep(10);//10us
                        }
                        if (fd_closed)
                            continue;


                        printf("recv msg_size: %d\n", msg_size);
                        //2. recv msg content
                        char* buf = new char[msg_size];
                        bytes = 0;
                        while (bytes < msg_size)
                        {
                            int ret = recv(fd, (char*)buf + bytes, msg_size - bytes, 0);
                            if (ret <= 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != ETIMEDOUT)
                            {
                                FD_CLR(fd, &_readset);
                                ::close(fd);
                                printf("ret = %d, errno: %d, closing the connect fd:%d\n", ret, errno, fd);
                                fd_closed = true;
                                break;
                            }

                            bytes += ret;
                            usleep(10); //10us
                        }
                        if (fd_closed)
                            continue;

                        //echo back
                        ::send(fd, &msg_size, sizeof(msg_size), 0);
                        ::send(fd, buf, msg_size, 0);
                        printf("recv msg content, %s ok.\n", buf);
                    }
                }
            }
        } //while(running)

        _running = false;
        printf("serving end.\n");
    }

};

