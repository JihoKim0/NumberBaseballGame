#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF 100
#define SIZE 3
#define size 10

int baseball(char* answer, char* num) {
    int result;
    int strike = 0, ball = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (num[i] == answer[j]) {
                if (i == j)
                    strike++;
                else
                    ball++;
                break;
            }
        }
    }

    result = strike * 10 + ball;
    return result;
}


int main(int argc, char** argv) {
    char msg[size];
    int str_len;
    int count = 9;                  //게임 기회
    int a[size] = { NULL }; //정답을 저장할 정수배열
    char answer[size] = { NULL };   //정답을 저장할 문자열
    int result;                                     //판별 결과를 저장할 정수
    char r[size] = { NULL };        //판별 결과를 저장할 문자열

    int serv_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    //야구게임 답
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        a[i] = rand() % 9 + 1;
        for (int j = 0; j < i; j++) {
            if (a[i] == a[j]) {
                i--;
                break;
            }
        }
    }

    //정답 문자열로 변환
    for (int i = 0; i < SIZE; i++) {
        answer[i] = a[i] + '0';
    }
    printf("answer : %s\n", answer);

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    while (1) {
        clnt_addr_size = sizeof(clnt_addr);
        str_len = recvfrom(serv_sock, msg, BUF, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        printf("msg : %s\n", msg);

        //야구게임 코드
        result = baseball(answer, msg); //정답 판별
        r[0] = result / 10 + '0'; //정답을 문자열로 변환
        r[1] = result % 10 + '0';
        strcpy(msg, r);

        if (strcmp(msg, "30") == 0)      //정답
            strcpy(msg, "success");

        count--;        //횟수 1회 차감

        if (count == 0)                                 //실패
            strcpy(msg, "fail");

        sendto(serv_sock, msg, str_len, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));

    }

    close(serv_sock);

    return 0;
}