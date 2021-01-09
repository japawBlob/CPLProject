#include "chw09.h"

doubleLinkedList* mainList;

doubleLinkedList* includeFile(char* file);

void* executeCommandsFromFile(void* file){
	printf("Your Input: %s\n", (char*) file);
	doubleLinkedList* mainList = includeFile((char*) file);
    node* current = mainList->head;
    while(current != NULL){
        printf("%i    %s\n", current->index, current->string);
        // printf("%i    ",current->index);
        // for (int i = 0; i < strlen(current->string); ++i)
        // {
        //  	printf("%x ", current->string[i]);
        // } printf("\n");
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

	int i = 0;
	char ret [255] = "";
	while(current != NULL){
		if (current->string[0] != '#'){
			printf("Sending %i    %s\n", current->index, current->string);
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
					//printf("the glob response is: %s\n", glob_response);
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
			}
			printf("sleep end, ret: %s\n", ret);
		}
		if (strstr(current->string, "#if:") == NULL){
			if(strstr(current->string, "goto") != NULL){
				char* label = strstr(current->string, "goto:");
				label += strlen("goto");
				char* temp = strstr(label, "\n");
				if(temp != NULL){
					*temp = '\0';
				}
				printf("%s\n", label);
				current = mainList->head;
				while(current->next != NULL){
					char temp [200];
					sprintf(temp, "#label%s", label);
					/*printf("SEARCHING:%s\nHAVE:%i   %s\n", temp, current->index, current->string);*/
					if(strstr(current->string, label) != NULL && strstr(current->string, "#label") != NULL){
						printf("here\n");
						break;
					}
					current = current->next;
				}
			}
		} else {
			printf("%s\n", current->string);
			char label [50];
			char condition [50];
			sscanf(current->string, "#if:%[^:]:goto:%[^:]:", condition, label);
			printf("condition: %s, label: %s, ret: %s\n", condition, label, ret);
			if(strncmp(condition, ret, strlen(condition)) == 0){
				printf("goto\n");
			}
		}
		if (strstr(current->string, "#exit:") != NULL){
			break;
		}
		current = current->next;
		i++;
		if(i>20) break;
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