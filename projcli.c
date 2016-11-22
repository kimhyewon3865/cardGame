#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void gotoxy(int x,int y);
void printTable();

char result[16] = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h'};


int main() {
	// char result[16] = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h'};
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
	printTable();

}

void gotoxy(int x,int y)
{
	printf("%c[%d;%df",0x1B,y,x);
}

void printTable() {
	int i, j = 3, k = 3;

	for (int i = 0; i < 16; ++i)
	{
		gotoxy(j,k); //reposition cursor
		printf("%c",result[i]);
		j = j + 2;
		if ((i+1)%4 == 0)
		{
			printf("\n");
			k = k + 2;
			j= 3;
		}
	}
}
