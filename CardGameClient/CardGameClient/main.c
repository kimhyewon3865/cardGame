//
//  main.c
//  CardGameClient
//
//  Created by 김혜원 on 2016. 11. 23..
//  Copyright © 2016년 김혜원. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// command to server
#define COMMAND_GAME_START "start"
#define COMMAND_SELECT "select"

// command from server
#define COMMAND_CONNECT "connect"
#define COMMAND_GAME_INFO "game_info"
#define COMMAND_FINISH "finish"

void waitInput(int socket, int maxfdp1, fd_set *read_fds);
char* readDataFromServer(int socket);
void processData(char*);
void parseData(char **result, char *data);
void splitString(char *result[], char string[], char delimiter[]);

/////////////////////////

#define MAXLINE 511
#define NAME_LEN 20

int tcp_connect(int af, char *servip, unsigned short port);
void errquit(char *mesg) { perror(mesg); exit(1); }

void gotoxy(int x,int y);
void printTable();
char * inputPoint();
void makeCardTable();
void overTurn(char *);

char *result;
int cardState[16] = {0};
char *EXIT_STRING = "exit";


const char *GAME_STARAT_STRING = "start";



int main(int argc, char * argv[]) {
    // char bufall[MAXLINE + NAME_LEN];//, *bufmsg;
    int maxfdp1, socket; //, namelen;
    fd_set read_fds;
    
//    if(argc != 4) {
//        printf("사용법: %s  server_ip port name \n", argv[0]);
//        exit(0);
//    }
    
    argv[1] = "125.209.199.157";
    argv[2] = "80";
    argv[3] = "뿅알";
    
    // open socket
    socket = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
    if(socket == -1) {
        errquit("서버 접속에 실패했습니다.");
    }
    puts("서버에 접속되었습니다.");
    maxfdp1 = socket + 1;
    FD_ZERO(&read_fds);
    
    while (1) {
        
        //printf("wait...");
        
        waitInput(socket, maxfdp1, &read_fds);
        
        if (FD_ISSET(socket, &read_fds)) {
            char *dataFromServer = readDataFromServer(socket);
            if (dataFromServer != NULL) {
                printf("data: %s\n", dataFromServer);
                processData(dataFromServer);
            }
            
        } else if (FD_ISSET(0, &read_fds)) {
            char *bufmsg;
            if (fgets(bufmsg, MAXLINE, stdin)) {
                printf("your input is %s\n",bufmsg);
            }
            
            
            
            // TODO: processKeyboardInput()
        }
    }
    
    
    
    
    
    
    //////////////////////////////////////////
    
//    sprintf(bufall, "[%s] : ", argv[3]);
//    
//    namelen = strlen(bufall);
//    
//    bufmsg = bufall + namelen;
    
//    socket = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
    
//    if(socket == -1)
//        errquit("tcp_connet fail");
    
//    puts("서버에 접속되었습니다.");
//    maxfdp1 = socket + 1;
//    FD_ZERO(&read_fds);
    
//    while(1) {
//        
////        FD_SET(0, &read_fds);
////        FD_SET(socket, &read_fds);
//
////        if(select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0 ) {
////            errquit("select fail");
////        }
//        
//        if(FD_ISSET(socket, &read_fds)) {
//            int nbyte;
//            if( (nbyte = recv(socket, bufmsg, MAXLINE, 0)) > 0) {
//                bufmsg[nbyte] = 0;
//                result = bufmsg;
//                printTable();
//                printf("%s \n", bufmsg);
//            }
//        }
//        
//        if(FD_ISSET(0, &read_fds)) {
//            printf("x1, y1, x2, y2 입력:\n");
//            if(fgets(bufmsg, MAXLINE, stdin)) {
//                if(send(socket, bufall, namelen + strlen(bufmsg),0)<0)
//                puts("Error : Write error on socket.");
//                if(strstr(bufmsg, EXIT_STRING) != NULL) {
//                    puts("Good bye.");
//                    close(socket);
//                    exit(0);
//                }
//            }
//        }
//    }
    
    
    
    
    
    
    
}

//////////////////////

void waitInput(int socket, int maxfdp1, fd_set *read_fds) {
    FD_SET(0, read_fds);
    FD_SET(socket, read_fds);
    if(select(maxfdp1, read_fds, NULL, NULL, NULL) < 0 ) {
        errquit("select fail");
    }
}

char* readDataFromServer(int socket) {
    char *bufmsg = NULL;
    long nByte = recv(socket, bufmsg, MAXLINE, 0);
    if (0 < nByte) {
        bufmsg[nByte] = 0;
        return bufmsg;
    } else {
        return NULL;
    }
}

void processData(char* data) {
    if (strstr(data, COMMAND_CONNECT) != NULL) {
        char *result[1] = { "" };
        parseData(result, data);
        // TODO: processConnect(result);
    } else if(strstr(data, COMMAND_GAME_INFO) != NULL) {
        char *result[6] = { "" };
        parseData(result, data);
        // TODO: processGameInfo(result);
    } else if(strstr(data, COMMAND_FINISH) != NULL) {
        char *result[1] = { "" };
        parseData(result, data);
        // TODO: processFinish(result);
    }
}

void parseData(char *result[], char *data) {
    char *firstArray[2];
    char firstDelimiter[] = ":";
    char secondDelimiter[] = "/";
    
    splitString(firstArray, data, firstDelimiter);
    splitString(result, firstArray[1], secondDelimiter);
}

void splitString(char *result[], char string[], char delimiter[]) {
    int i = 0;
    char *token = NULL;
    
    token = strtok(string, delimiter);
    while (token != NULL) {
        result[i] = token;
        token = strtok(NULL, delimiter);
        i++;
    }
}

//////////////////////

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

void makeCardTable() {
    char temp;
    int i;
    int randNumber;
    
    srand((unsigned)time(NULL));
    
    for(i=0; i<16; i++) {
        randNumber = rand()%16;
        temp = result[i];
        result[i] = result[randNumber];
        result[randNumber] = temp;
    }
}


void printTable() {
    int i, j = 3, k = 20;
    
    for (i = 0; i < 16; ++i)
    {
//        gotoxy(j,k);
        // if(cardState[i] != 0) {
        // 	printf("%c",result[i]);
        // } else {
        // 	printf("?");
        // }
        printf("%c",result[i]);
        j = j + 2;
        if ((i+1)%4 == 0) {
            printf("\n");
            k = k + 2;
            j= 3;
        }
    }
}

char * inputPoint() {
    char *bufmsg;
    int x1,y1,x2,y2;
    gotoxy(2,30);
    printf("첫번째카드 x 입력 : ");
    scanf("%d",&x1);
    *bufmsg = x1;
    printf("첫번째카드 y 입력 : ");
    scanf("%d",&y1);
    *bufmsg += y1;
    printf("두번째카드 x 입력 : ");
    scanf("%d",&x2);
    *bufmsg += x2;
    printf("두번째카드 x 입력 : ");
    scanf("%d",&y2);
    *bufmsg += y2;
    
    return bufmsg;
}

int tcp_connect(int af, char *servip, unsigned short port) {
    struct sockaddr_in servaddr;
    int s;
    if( (s = socket(af, SOCK_STREAM, 0)) < 0 )
    return -1;
    
    bzero( (char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = af;
    inet_pton(AF_INET, servip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    
    if(connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    return -1;
    
    return s;
}

//void overTurn(char *point) {
//    char * token = NULL;
//    char *s1;
//    char s2[] = ", ";
//    int pointArray[10];
//    int i=0,j;
//    *s1 = *point;
//    token = strtok(s1,s2);
//    while(token!=NULL)
//    {
//        pointArray[i++] = atoi(token);
//        printf("token = %s\n",token);
//        token = strtok(NULL, s2);
//        
//    }
//    for(j=0; j<i; i++)
//    printf("%d\n",pointArray);
//}
