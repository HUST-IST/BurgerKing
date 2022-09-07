#include <stdlib.h>
#include <stdio.h>
#define MAXSIZE 8

int ringbuf[MAXSIZE];
int realdx = 0;
int writeldx = 0;
int next_data_handle(int addr)
{
	return (addr + 1) == MAXSIZE ? 0 : (addr + 1);
}
int write_data(int data)
{
	int i;
	*(ringbuf + writeldx) = data;
	writeldx = next_data_handle(writeldx);
	for (i = 0; i < MAXSIZE; i++)
	{
		printf("%4d\t", *(ringbuf + i));
		if (MAXSIZE - 1 == i)
			printf("\n");
	}
}
int read_data()
{
	printf("read data is : %d\t", *(ringbuf + realdx));
	realdx = next_data_handle(realdx);
}

int main(int argc, char* argv)
{
	int data;
	char cmd;
	do {
		printf("select:\tw--write:\tr--read:\tq--quit\n");
		scanf("%s", &cmd);
		switch (cmd)
		{
		case 'w':
			printf("please input data:");
			scanf("%d", &data);
			write_data(data);
			break;
		case 'r':
			data = read_data();
			printf("read all \n");
			break;
		case 'q':
			printf("quit\n");
			break;
		default:
			printf("Command error\n");
			break;
		}
	} while (cmd != 'q');
	return 0;
}
