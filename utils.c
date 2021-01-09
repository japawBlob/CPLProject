#include "chw09.h"

char * recivieMessage(int hSerial){
    char *chArrBuf = (char*)malloc(sizeof(char)*255);
    memset (chArrBuf , '\0', 255 );
    int n = 0;
    //while(strlen(chArrBuf) == 0 /*&& chArrBuf[n-2] != '\n' && chArrBuf[n-1] != '\r'*/){
    usleep(1000*100);
    n = read( hSerial, chArrBuf, 255);
    //}
    if (strlen(chArrBuf) >= 2)
    {
    	if (chArrBuf[n-2] == '\n' && chArrBuf[n-1] == '\r'){
    		chArrBuf[n-2] = '\0';
    		return chArrBuf;
    	}
    	
    }
    free(chArrBuf);
    return NULL;
}

int sendMessage(int hSerial, char* input){
	char payload [255];
	sprintf(payload, "%s\n\r", input);
    int n_written = write( hSerial, payload, strlen(payload)); 
    return n_written;
}

static const char* menu[] = {"== program menu ==",
			"Item 1: Control LED",
			"Item 2: Read button state",
			"Item 3: Read joystick",
			"Item 4: Call execution of file",
			"Item c: Enter a custom command",
			"Item h: Display help",
			"Item e: Exit",
			"Selection:"};

void printMenu(){
	int size = (sizeof(menu)/sizeof(char*));
	//printf("%i\n", size);
	for (int i = 0; i < size; ++i){
		printf("%s\n", menu[i]);
	}
}


void flushStdin (){
	char ch;
	while(EOF!=(ch=fgetc(stdin)) && ch != '\n'){
	}
}