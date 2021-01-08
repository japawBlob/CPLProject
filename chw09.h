#include <stdio.h>
#include <stdlib.h>
 
#include <termios.h>
#include <fcntl.h>
 
#include <string.h>
#include <unistd.h>
 
#include <pthread.h>
#include <semaphore.h>

#include <time.h>

typedef struct node {
	int index;
	struct node* next;
	struct node* previous;
	char* string;
} node;

typedef struct {
	node* head;
	node* tail;
} doubleLinkedList;

int main(int argc, char const *argv[]);
char * recivieMessage(int hSerial);
int sendMessage(int hSerial, char input[]);


int linked(int argc, char const *argv[]);
doubleLinkedList* includeFile(char* file);