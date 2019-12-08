
#include <stdio.h>
#include "async_tcp_server.h"
#include <unistd.h>

int main(void)
{
    async_tcp_server server("127.0.0.1", 9999);
    server.start();

    server.waitforshutdown();
    printf("server is running...\n");
}
