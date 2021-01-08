#include <stdio.h>
#include <stdlib.h>
 
#include <termios.h>
#include <fcntl.h>
 
#include <string.h>
#include <unistd.h>
 
#include <pthread.h>
#include <semaphore.h>

#include <time.h>

char * recivieMessage(int hSerial){
    char *chArrBuf = (char*)malloc(sizeof(char)*255);
    memset (chArrBuf , '\0', 255 );
    int n = 0;
    while(strlen(chArrBuf) == 0 /*&& chArrBuf[n-2] != '\n' && chArrBuf[n-1] != '\r'*/){
        usleep(1000*100);
        n = read( hSerial, chArrBuf, 255);
    }
    chArrBuf[n-2] = '\0';
    return chArrBuf;
}

int main(int argc, char const *argv[]){
    printf("program started\n");
    const char* serialPort;
    if(argc < 2){
        serialPort = "/dev/ttyACM0";
    } else {
        serialPort = argv[1];
    }
 
    int hSerial = open( serialPort, O_RDWR| O_NONBLOCK | O_NDELAY );
 
    struct termios o_tty;
    memset (&o_tty, 0, sizeof o_tty);
    int iRetVal = tcgetattr (hSerial , &o_tty);
 
 
    /* set raw input, 1 second timeout */
    o_tty.c_cflag     |= (CLOCAL | CREAD);
    o_tty.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    o_tty.c_oflag     &= ~OPOST;
    o_tty.c_cc[VMIN]  = 0;
    o_tty.c_cc[VTIME] = 10;
 
    o_tty.c_cflag &= ~PARENB;
    o_tty.c_cflag &= ~CSTOPB;
    o_tty.c_cflag &= ~CSIZE;
    o_tty.c_cflag |= CS8;

    cfsetispeed(&o_tty, B9600);
    cfsetispeed(&o_tty, B9600);
 
    /* set the options */
    tcsetattr(hSerial, TCSANOW, &o_tty);

    
    while(1){
        char* c = recivieMessage(hSerial);
        printf("%s\n", c);
        if (strstr(c, "SEL") != NULL)
        {
            free(c);
            break;
        }
        free(c);
    }
    
    close(hSerial);
 
    return 0;
}