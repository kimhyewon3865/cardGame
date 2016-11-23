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
char *GAME_STARAT_STRING = "start";


int maxfdp1;
int num_chat = 0;
int clisock_list[MAX_SOCK];
int listen_sock;

char result[16] = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h'};
int cardState[16] = {0};


void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeClient(int s);
int tcp_listen(int host, int port, int backlog);
void errquit(char *mesg) {perror(mesg); exit(1);}
void makeCardTable();

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	char buf[MAXLINE+1];
	int i,j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in);
	fd_set read_fds;

	if(argc != 2) {
		printf("사용법: %s port\n", argv[0]);
		exit(0);
	}

	//tcp_listen(host,port,backlog)함수호출
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
			accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);

			if(accp_sock == -1)
				errquit("accept fail");

			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0);
			printf("%d번째 사용자 추가\n",num_chat);
		}
		//클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
		for(i = 0; i < num_chat; i++){
			if(FD_SET(clisock_list[i], &read_fds)) {
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);

				if(nbyte <= 0){
					removeClient(i);
					continue;
				}

				buf[nbyte] = 0;
				//종료문자 처리
				if(strstr(buf, EXIT_STRING) != NULL) {
					removeClient(i);
					continue;
				}
				//게임시작 처리
				if(strstr(buf, GAME_STARAT_STRING) != NULL) {
					//랜덤돌리기
					makeCardTable()
					printf("%s\n",result);
					//랜덤 방송
					send(clisock_list[j], result, sizeof(result), 0);
					continue;
				}


				//모든 채팅 참가자에게 메시지 방송
				for(j = 0; j < num_chat; j++)
					send(clisock_list[j], buf,nbyte,0);

				printf("%s\n",buf);
			}
		}
	}//end of while
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

	for(i=0; i<16; i++) {
		randNumber = rand()%16;
		temp = result[i];
		result[i] = result[randNumber];
		result[randNumber] = temp;
	}
}