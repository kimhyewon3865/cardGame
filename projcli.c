#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
	char result[16] = {'a','a','b','b','c','c','d','d','e','e','f','f','g','g','h','h'};
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

	for (int i = 0; i < 16; ++i)
	{
		printf("%c",result[i]);
		if ((i+1)%4 == 0)
		{
			printf("\n");
		}
	}
}