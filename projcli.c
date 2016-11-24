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

int main(int argc, char * argv[]) {
	char bufall[MAXLINE + NAME_LEN], *bufmsg;
	int maxfdp1, s, namelen;
	fd_set read_fds;

	if(argc != 4) {
		printf("사용법: %s  server_ip port name \n", argv[0]);
		exit(0);
	}
	puts("a");
	sprintf(bufall, "[%s] : ", argv[3]);
		puts("b");

	namelen = strlen(bufall);
		puts("c");

	bufmsg = bufall + namelen;
	puts("d");

	s = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
	puts("e");

	if(s == -1) 
		errquit("tcp_connet fail");

	puts("서버에 접속되었습니다.");
	maxfdp1 = s + 1;
	FD_ZERO(&read_fds);

	while(1) {
		FD_SET(0, &read_fds);
		FD_SET(s, &read_fds);
		if(select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0 )
			errquit("select fail");
		if(FD_ISSET(s, &read_fds)) {
			int nbyte;
			if( (nbyte = recv(s, bufmsg, MAXLINE, 0)) > 0) {
				bufmsg[nbyte] = 0;
				if(strstr(buf, GAME_STARAT_STRING) != NULL) {
					result = bufmsg;
					printTable();
				} else {
					overTurn(bufmsg);
				}		
				printf("%s \n", bufmsg);
			}
		}

		if(FD_ISSET(0, &read_fds)) {
			printf("x1, y1, x2, y2 입력:\n");
			if(fgets(bufmsg, MAXLINE, stdin)) {
				if(send(s, bufall, namelen+strlen(bufmsg),0)<0)
					puts("Error : Write error on socket.");
				if(strstr(bufmsg, EXIT_STRING) != NULL) {
					puts("Good bye.");
					close(s);
					exit(0);
				}
			}
		}
	}

}

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
		gotoxy(j,k); 
		if(cardState[i] != 0) {
			printf("%c",result[i]);
		} else {
			printf("?");
		}	
			j = j + 2;
			if ((i+1)%4 == 0)
			{
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

void overTurn(char *point) {
	char * token = NULL;
 	char *s1 = *point;
 	char s2[] = ", ";
 	int pointArray[10];
 	int i=0,j;
 	token = strtok(s1,s2);
	 while(token!=NULL)
	 {
		pointArray[i++] = atoi(token);
		printf("token = %s\n",token);
	  	token = strtok(NULL, s2);

	 }
	 for(j=0; j<i; i++)
	 	printf("%d\n",pointArray);
}
