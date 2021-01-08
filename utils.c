#include "chw09.h"

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

int sendMessage(int hSerial, char input[]){
    int n_written = write( hSerial, input, strlen(input)); 
    return n_written;
}