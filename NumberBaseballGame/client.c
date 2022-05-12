#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF 100

int main(int argc, char** argv) {
    int sock;
    char msg[BUF];
    int str_len;

    struct sockaddr_in serv_addr;

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("sock error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while (1) {
        fputs("Enter a message (q to quit): ", stdout);
        fgets(msg, sizeof(msg), stdin);

        if (!strcmp(msg, "q\n")) {
            break;
        }

        write(sock, msg, strlen(msg));


        str_len = read(sock, msg, sizeof(msg) - 1);
        msg[str_len] = 0;

        if (strcmp(msg, "succ") == 0) {
            printf("success\n");
            break;
        }
        if (strcmp(msg, "fail") == 0) {
            printf("fail\n");
            break;
        }

        printf(": %cS%cB\n", msg[0], msg[1]);
    }

    close(sock);

    return 0;
}
