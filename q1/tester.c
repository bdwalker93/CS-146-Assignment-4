#include<stdio.h>

int main(int argc, char *argv[])
{

	printf("This is Hello World\n");

	int i;
	for(i=0; i<1; i++)
		printf("%d\n", i); 
	for(i = 0; i < argc; i++)
		printf("-- %s\n", argv[i]);
}
