#include "chw09.h"

doubleLinkedList* mainList;

doubleLinkedList* includeFile(char* file);

void* executeCommandsFromFile(void* file){
	printf("Your Input: %s\n", (char*) file);
	doubleLinkedList* mainList = includeFile((char*) file);
    node* current = mainList->head;
    while(current != NULL){
        //printf("%i    %s\n", current->index, current->string);
        current = current->next;
    }
    current = mainList->head;
    executeLinked(mainList);
    

    current = mainList->head;
    while(current != NULL){
        node* temp = current;
        current = current->next;
        free(temp->string);
        free(temp);
    }
    free(mainList);
	pthread_mutex_unlock(&readyToJoinExecutinThread);
	free(file);
	return NULL;
}
int executeLinked(doubleLinkedList* mainList)
{	
	node* current = mainList->head;

	char ret [255] = "";
	while(current != NULL){
		if (current->string[0] != '#'){
			//printf("Sending %i    %s\n", current->index, current->string);
			pthread_mutex_lock(&messageLock);
	        sprintf(glob_message, "%s", current->string);
			pthread_mutex_unlock(&messageReady);

			current = current->next;
			continue;
		}
		if (strstr(current->string, "#wait_for_joystick") != NULL)
		{
			int timeout = -1;
			int infinity = 0;
			if(sscanf(current->string, "#wait_for_joystick:%i:%[^:]:", &timeout, ret) < 2){
				infinity = 1;
			}
			printf("Waiting for joystick input. Timeout: %ims\n", timeout);
			pthread_mutex_unlock(&needResponse);
			while(timeout>0 || infinity){
				if (pthread_mutex_trylock(&responseLock) == 0)
				{
					if (strstr(glob_response, "JOY_") == NULL)
					{
						pthread_mutex_unlock(&needResponse);
					} else {
						strcpy(ret, glob_response);
						break;
					}
				}
				usleep(1000);
				timeout--;
				if(pthread_mutex_trylock(&endProgram) == 0){
					pthread_mutex_unlock(&endProgram); 
					break;
				}
			}
			//printf("sleep end, ret: %s\n", ret);
		}
		if (strstr(current->string, "#if:") == NULL){
			if(strstr(current->string, "goto") != NULL){
				char* label = strstr(current->string, "goto:");
				label += strlen("goto");
				char* temp = strstr(label, "\n");
				if(temp != NULL){
					*temp = '\0';
				}
				//printf("%s\n", label);
				current = mainList->head;
				while(current != NULL){
					char temp [200];
					sprintf(temp, "#label%s", label);
					if(strstr(current->string, label) != NULL && strstr(current->string, "#label") != NULL){
						//printf("here\n");
						break;
					}
					current = current->next;
				}
			}
		} else {
			char label [50];
			char condition [50];
			while(strstr(current->string, "#if:") != NULL){
				//printf("%s\n", current->string);
				
				sscanf(current->string, "#if:%[^:]:goto:%[^:]:", condition, label);
				//printf("condition: %s, label: %s, ret: %s\n", condition, label, ret);
				//printf("cur->next->string %s\n", current->next->string);
				if(strncmp(condition, ret, strlen(condition)) == 0){
					node* temp = current;
					current = mainList->head;
					while(current != NULL){
						char temp [200];
						sprintf(temp, "#label:%s:", label);
						//printf("SEARCHING:%s\nHAVE:%i   %s\n", temp, current->index, current->string);
						if(strncmp(temp, current->string, strlen(temp)) == 0){
							goto skipElse;
						}
						current = current->next;
					}
					if(current == NULL){
						printf("Label not found\n");
						current = temp;
					}
				}
				current = current->next;
			}
			while(strncmp(current->string,"#else:", strlen("#else:")) != 0){
				current = current->next;
			}
			//printf("else is: %s\n", current->string);
			continue;
		}
		skipElse:
		if (strstr(current->string, "#exit:") != NULL){
			printf("FIle execution ended\n");
			break;
		}
		current = current->next;
		if(pthread_mutex_trylock(&endProgram) == 0){
			pthread_mutex_unlock(&endProgram); 
			break;
		}
	}
	return 0;
}
doubleLinkedList* includeFile(char* file){
	doubleLinkedList* list = (doubleLinkedList*) malloc (sizeof(doubleLinkedList));
	list->head = NULL;
	list->tail = NULL;
	FILE * f = fopen(file, "r");
	if (f == NULL)
	{
		printf("Cant find that file\n");
		free(list);
		return NULL;
	}
	char c;
	node* current;
	node* previous;
	int position = 1;
	while((c = fgetc(f)) != EOF){
		current = (node*) malloc (sizeof(node));
		char* buf = (char*) malloc (sizeof(char)*100);
		int i = 0;
		while(c != '\n'){
			buf[i++] = c;
			c = fgetc(f);
		}
		buf[i] = '\0';
		current->string = buf;
		char* ptr;
		if (strstr(buf, "//") != NULL || current->string[0] == 0xd){
			free(current->string);
			free(current);
			continue;
		}
		if((ptr = strstr(buf, "#include:")) != NULL){
			char newFileName[100];
			sscanf(buf,"#include:%99[^ =:]:", newFileName);
			doubleLinkedList* tempList;
			tempList = includeFile(newFileName);

			previous->next = tempList->head;
			tempList->head->previous = previous;

			previous = tempList->tail;
			list->tail = tempList->tail;
			free(tempList);
			node* temp = current;
			current = list->tail->next;
			free(temp->string);
			free(temp);

			// previous->next = current;
			// current->previous = previous;

			// previous = tempList->tail;
			// current->next = tempList->head;
			// tempList->head->previous = current;
			// list->tail = tempList->tail;
			// free(tempList);
			// current -> index = position++;
			continue;
		}
		if(list->head == NULL){
			current->previous = NULL;
			list->head = current;
			previous = current;
		} else {
			previous->next = current;
			current->previous = previous;
			previous = current;
			current->next = NULL;
			list->tail = current;
		}
		current -> index = position++;

	}
	fclose(f);
	return list;
}