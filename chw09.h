#include <stdio.h>
#include <stdlib.h>
 
#include <termios.h>
#include <fcntl.h>
 
#include <string.h>
#include <unistd.h>
 
#include <pthread.h>
#include <semaphore.h>

#include <time.h>

enum commandOptions{LED_ON, LED_OFF, BUTTON, JOYSTICK};

pthread_mutex_t messageReady;
pthread_mutex_t messageLock;
pthread_mutex_t endProgram;
pthread_mutex_t fileExecutionReady;
pthread_mutex_t readyToJoinExecutinThread;
pthread_mutex_t needResponse;
pthread_mutex_t responseLock;

char glob_message[255];
char glob_response[255];

int prom;

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

int hSerial;

void printMenu();
void flushStdin ();
int echo ();

int main(int argc, char const *argv[]);
char * recivieMessage(int hSerial);
int sendMessage(int hSerial, char input[]);

void* executeCommandsFromFile(void* file);
int executeLinked(doubleLinkedList* mainList);
doubleLinkedList* includeFile(char* file);

void* communication(void* blob);