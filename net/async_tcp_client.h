#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <stddef.h>
#include <thread>
#include <arpa/inet.h>
#include <string.h>

using namespace std;


typedef struct client_msg
{
    int size;
    char data[0];
} client_msg;

class async_tcp_client
{
private:
    int _fd = -1;
    int _error = 0;
    struct sockaddr_in _addr;

    thread* _run_thread = nullptr;
    volatile bool _running = false;
    bool _connected = false;

public:
    async_tcp_client(const string& host, short port, bool nodelay=true) 
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
            printf("create socket failed, errno: %d\n", errno);
            _error = errno;
            return;
        }

        //SOL_TCP
        if (nodelay)
        {
            int enable = 1;
            if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0)
            {
                fprintf(stderr, "TCP_NODELAY set failed, errno: %d\n", errno);
                _error = errno;
                return;
            }
        }

        //set addr
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("0.0.0.0");
        addr.sin_port = htons(port);
        
        _fd = fd;
        _addr = addr;
        //set non-blocking
        set_nonblocking(_fd);
    }

    ~async_tcp_client() 
    {
        stop();
        delete _run_thread;
    }


    void stop()
    {
        if (_running)
            _running = false;
        //else
        //    return;
        
        //join thread
        if (_run_thread)
            _run_thread->join();

        
        //close socket
        if (_fd > 0)
        {
            close(_fd);
            _fd = -1;
        }

        printf("client stopped.\n");
    }

    void start()
    {
        if (_running)
            return;
        
        _running = true;
        //start select thread
        _run_thread = new thread(thread_func, this);
    }

    void waitforshutdown()
    {
        if (_running)
        {
            printf("waiting for shutdown...\n");
            _run_thread->join();
            delete _run_thread;
            _run_thread = nullptr;
            _running = false;
            close(_fd);
            _fd = -1;
        }
        else
            printf("already shutdown.\n");
        
        printf("client shutdown\n");
    }

    int connect()
    {
        if (_running && _connected)
            return 0;

        if (::connect(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) < 0)
        {
            if (EINPROGRESS == errno || EINTR == errno || EWOULDBLOCK == errno)
                return 0;
            
            printf("errno: %d, when ::connect", errno);
            
            return -1;
        }

        return 0;
    }

    bool is_connected()
    {
        return _connected;
    }

    int send_msg()
    {
        if (_running && _connected)
        {
            char data[] = "helloworld";
            int len = sizeof(data);
            char *msg = new char[(sizeof(int)+len)];
            client_msg *hello = (client_msg*)msg;
            hello->size = len;
            memcpy(hello->data, data, len);

            ::send(_fd, msg, sizeof(int)+len, 0);
            printf("client send a helloworld msg, size: %d\n", sizeof(int) + len);
        }

        return 0;
    }

    int recv_msg();    

private:
    static void thread_func(async_tcp_client *client)
    {
        client->run();
    }

    int create_async_socket();

    int set_nonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        return fcntl(fd, F_SETFL, new_option);
    }

    void run()
    {
        fd_set readset;
        fd_set writeset;
        fd_set testset;

        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_ZERO(&testset);

        FD_SET(_fd, &writeset);
        int ret = 0;

        struct timeval interval{0, 10000};//10ms
        while (_running)
        {
            fd_set testreadset = readset;
            fd_set testwriteset = writeset;

            ret = select(_fd + 1, &testreadset, &testwriteset, NULL, &interval);
            if (ret < 0) //return 0 when expire
            {
                perror("select _error");
                _error = errno;
                close(_fd);
                _fd = -1;
                break;
            }

            
            //check writeable fd
            if (!_connected)
            {
                if (FD_ISSET(_fd, &testwriteset))
                { 
                    int so_error = 0;
                    socklen_t len = 0;
                    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0)
                    {
                        perror("getsockopt failed, break");
                        _error = errno;
                        close(_fd);
                        _running = false;
                    }

                    if (so_error  == 0)
                    {
                        printf("----- connected to server -----\n");
                        _connected = true;
                        FD_ZERO(&writeset);
                        FD_SET(_fd, &readset);
                    }
                    else
                    {
                        if (so_error == EINPROGRESS || so_error == EWOULDBLOCK)
                        {
                            //waiting for connect ok state
                            continue;
                        }
                        else
                        {
                            printf("so_error: %d , break", so_error);
                            _error = errno;
                            close(_fd);
                            _running = false;
                        }
                    }
                }
            }
            //already connected, waiting for message
            else
            {
                if (FD_ISSET(_fd, &testreadset))
                {
                    //1. recv msg size
                    int msg_size = 0;
                    int bytes = 0;
                    int len = sizeof(msg_size);

                    while (bytes < len)
                    {
                        int ret = recv(_fd, ((char *)(&msg_size)) + bytes, sizeof(msg_size) - bytes, 0);
                        if (ret <= 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != ETIMEDOUT)
                        {
                            FD_CLR(_fd, &readset);
                            ::close(_fd);
                            printf("errno: %d, closing the connect fd:%d\n", errno, _fd);
                            _running = false;
                            return;
                        }

                        bytes += ret;
                        usleep(10); //10us
                    }

                    //2. recv msg content
                    char *buf = new char[msg_size];
                    bytes = 0;
                    while (bytes < msg_size)
                    {
                        int ret = recv(_fd, buf + bytes, msg_size - bytes, 0);
                        if (ret <= 0)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ETIMEDOUT)
                            {
                                usleep(10); //10us
                                continue;
                            }
                            else
                            {
                                FD_CLR(_fd, &readset);
                                ::close(_fd);
                                printf("errno: %d, closing the connect fd:%d\n", errno, _fd);
                                return;
                            }
                        }
                        
                        bytes += ret;
                    }

                    printf("#### recv a msg: %s, size: %d ok.\n", buf, msg_size);
                }
            }
        }
    }
};