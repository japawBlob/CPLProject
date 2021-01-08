#include <stdio.h>
#include <stdlib.h>
 
#include <termios.h>
#include <fcntl.h>
 
#include <string.h>
#include <unistd.h>
 
#include <pthread.h>
#include <semaphore.h>

#include <time.h>

int main(int argc, char const *argv[]);
char * recivieMessage(int hSerial);
int sendMessage(int hSerial, char input[]);