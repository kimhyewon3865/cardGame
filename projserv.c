#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit";
char *START_STRING = "Conneted to chat_server\n";
// char *GAME_STARAT_STRING = "start";


int maxfdp1;
int num_chat = 0;
int clisock_list[MAX_SOCK];
int listen_sock;



void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeClient(int s);
int tcp_listen(int host, int port, int backlog);
void errquit(char *mesg) {perror(mesg); exit(1);}
void makeCardTable();


///
// command from client
#define COMMAND_GAME_START "start"
#define COMMAND_SELECT "select"

// command to client
#define COMMAND_CONNECT "connect"
#define COMMAND_GAME_INFO "game_info"
#define COMMAND_FINISH "finish"
//
void makeCommandString(char *bufmsg, char *command, char *bufall);
void sendGameInfo();
void parseData(char *result[], char *data);
void splitString(char *result[], char string[], char delimiter[]);

//gameInfo 
int gameState = 0; //0:대기 1:게임중 2:종료 
char cardArray[17] = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h','\0'};
char cardState[17] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','\0'};
int nowTurn = 1; //1:첫번째참가자 2:두번째참가자
int correctState1 = 0;  //첫번째 참가자가 맞춘개수 
int correctState2 = 0;  //두번째 참가자가 맞춘개수


int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	char buf[MAXLINE + 1];
	char message[MAXLINE];
	int i,j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
	fd_set read_fds;

	if(argc != 2) {
		printf("사용법: %s port\n", argv[0]);
		exit(0);
	}

	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 2);
	maxfdp1 = getmax() + 1; 

	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);

		for(i=0; i < num_chat; i++)
			FD_SET(clisock_list[i], &read_fds);

		puts("wait for client");


		if(select(maxfdp1, &read_fds, NULL,NULL,NULL) < 0)
			errquit("select fail");

		if(FD_ISSET(listen_sock, &read_fds)) {
			char bufall[1024];
			accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);

			if(accp_sock == -1)
				errquit("accept fail");

			addClient(accp_sock, &cliaddr);
			//1) 
			//char*bufmsg = num_chat;
			//char * bufall = makeCommandString(bufmsg, "connect");
			//send(bufall);
			sprintf(buf, "%d", num_chat);
            makeCommandString(buf, COMMAND_CONNECT, bufall);

			send(accp_sock, bufall, strlen(bufall), 0);
			//send(accp_sock, START_STRING, strlen(START_STRING), 0);
			printf("%d번째 사용자 추가\n",num_chat);
		}
		for(i = 0; i < num_chat; i++){
			if(FD_SET(clisock_list[i], &read_fds)) {
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);

				if(nbyte <= 0){
					removeClient(i);
					continue;
				}

				buf[nbyte] = 0;
				printf("recv: %s\n", buf);

				if (strstr(buf, COMMAND_GAME_START) != NULL) {
					if (gameState == 0) {
						makeCardTable();
						gameState = 1;
						//sendGameInfo(); 
					}
				}


				//6
				// if (gamestate==2) {
				// 	if(correctState[0]>correctState[1]) {
				// 		strcat(bufmsg, 1);
				// 		sprintf(bufmsg,"%s/",correctState[0]);
				// 	} else {
				// 		strcat(bufmsg, 2);
				// 		sprintf(bufmsg,"%s/",correctState[1]);
				// 	}
				// } 
				// makeCommandString(bufmsg, "finish");


				if (strstr(buf, COMMAND_SELECT) != NULL) {
					char *result[4];
					int i1, i2, i;
					parseData(result, buf);

					for(i=0; i<16; i++) {
						if(cardState[i] == '2') {
							cardState[i] = '0';
						}
					}

					i1 = 4 * (atoi(result[0]) - 1) + (atoi(result[1]) - 1);
					i2 = 4 * (atoi(result[2]) - 1) + (atoi(result[3]) - 1);
					if( cardArray[i1] == cardArray[i2] ) { //같은그림 맞추면
						cardState[i1] = '1';
						cardState[i2] = '1';
						if (nowTurn == 1) {
							correctState1++;
						} else {
							correctState2++;
						}
						for (i = 0; i < 16; i++) {
							if (cardState[i] == '0') {
								gameState = 1;
								break;
							} else {
								gameState = 2;
							}
						}
					} else { //그림 틀리면 
						cardState[i1] = '2';
						cardState[i2] = '2';
					}
					if (nowTurn == 1) { 
						nowTurn = 2; 
					} else {
						nowTurn = 1;
					}
					//sendGameInfo(); 
				}




				if(strstr(buf, EXIT_STRING) != NULL) {
					removeClient(i);
					continue;
				}
				// if(strstr(buf, GAME_STARAT_STRING) != NULL) {
				// 	makeCardTable();
				// 	send(clisock_list[j], cardArray, sizeof(cardArray), 0);
				// 	continue;
				// }


				// for(j = 0; j < num_chat; j++)
				// 	send(clisock_list[j], buf,nbyte,0);
				//		send(clisock_list[j], bufall, strlen(bufall), 0);
				sendGameInfo();

				printf("%s\n",buf);
			}
		}
	}
	return 0;
}

void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf,sizeof(buf));
	printf("new client: %s\n",buf);
	clisock_list[num_chat] = s;
	num_chat++;
	maxfdp1 = getmax() + 1;
}

int getmax() {
	int max = listen_sock;
	int i;

	for(i=0; i < num_chat; i++)
		if(clisock_list[i] > max)
			max = clisock_list[i];

	return max;
}

void removeClient(int s) {
	close(clisock_list[s]);

	if(s != num_chat-1)
		clisock_list[s] = clisock_list[num_chat-1];

	num_chat--;
	printf("채팅 참가자 1명 탈퇴. 현재 참가자수 = %d\n", num_chat);
	maxfdp1 = getmax() + 1;

	gameState = 0; 
	//cardArray = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h','\0'};
	//cardState = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','\0'};
	nowTurn = 1;
	correctState1 = 0; 
	correctState2 = 0;
}

int tcp_listen(int host, int port, int backlog) {
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);

	if(sd == -1){
		perror("socket fail");
		exit(1);
	}

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);

	if(bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(1);
	}

	listen(sd, backlog);

	return sd;
}

void makeCardTable() {
	char temp;
	int i;
	int randNumber;

	srand((unsigned)time(NULL));

	for (i = 0; i < 16; i++) {
		randNumber = rand() % 16;
		temp = cardArray[i];
		cardArray[i] = cardArray[randNumber];
		cardArray[randNumber] = temp;
	}
}

///

void makeCommandString(char *bufmsg, char *command, char *bufall) {
    int namelen;
    sprintf(bufall, "%s:", command);
    namelen = strlen(bufall);
    //bufmsg = bufall + namelen;
    //printf("makeCommandString bufmsg: %s\n",bufmsg);
    strcat(bufall, bufmsg);
    //printf("makeCommandString bufall: %s\n",bufall);
}

void sendGameInfo() {
	char bufall[1024];
	int j = 0;
	sprintf(bufall, "game_info:%s/%s/%d/%d/%d/%d", cardArray, cardState, nowTurn, gameState, correctState1, correctState2);
	for(j = 0; j < num_chat; j++) {
		send(clisock_list[j], bufall, strlen(bufall), 0);
		printf("%d >>> %s\n", clisock_list[j], bufall);
	}
}

void parseData(char *result[], char *data) {
    char *firstArray[2];
    char firstDelimiter[] = ":";
    char secondDelimiter[] = ",";
    
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

