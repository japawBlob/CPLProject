#include "chw09.h"


void* communication(void* blob){

	while(1){
		//pthread_mutex_lock(&messageReady);
		if(pthread_mutex_trylock(&messageReady) == 0){
			sendMessage( hSerial, glob_message);
			pthread_mutex_unlock(&messageLock);
		}
		char* response = recivieMessage(hSerial);
		if (response != NULL)
		{
			printf("%s\n", response);
			if (pthread_mutex_trylock(&needResponse) == 0)
			{
				strcpy(glob_response, response);
				pthread_mutex_unlock(&responseLock);
			}
			free(response);
		}
		if(pthread_mutex_trylock(&endProgram) == 0){
			pthread_mutex_unlock(&endProgram); 
			break;
		}
	}
	
	return NULL;
}