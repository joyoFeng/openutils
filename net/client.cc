#include <stdio.h>
#include "async_tcp_client.h"
#include <unistd.h>

int main(void)
{
    async_tcp_client client("127.0.0.1", 9999);
    client.start();
    client.connect();

    usleep(1000000);
    client.send_msg();

    client.waitforshutdown();

    printf("client done.\n");
}
