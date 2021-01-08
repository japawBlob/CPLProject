#include "chw09.h"

doubleLinkedList* mainList;

doubleLinkedList* includeFile(char* file);

/**/
int executeLinked(doubleLinkedList* mainList)
{	
	node* current = mainList->head;

	int i = 0;
	while(current != NULL){
		printf("Sending %i    %s\n", current->index, current->string);
		sendMessage(hSerial, current->string);
		usleep(1000* 100);
		char* c = recivieMessage(hSerial);

		if(c != NULL){
            printf("%s\n", c);
            free(c);
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

		}
		if (strstr(current->string, "#exit:") != NULL){
			break;
		}
		current = current->next;
		i++;
		if(i>250) break;
	}
	return 0;
}
doubleLinkedList* includeFile(char* file){
	doubleLinkedList* list = (doubleLinkedList*) malloc (sizeof(doubleLinkedList));
	list->head = NULL;
	list->tail = NULL;
	FILE * f = fopen(file, "r");
	char c;
	node* current;
	node* previous;
	int position = 1;
	while((c = fgetc(f)) != EOF){
		current = (node*) malloc (sizeof(node));
		char* buf = (char*) malloc (sizeof(char)*100);
		int i = 0;
		while(c != '\n'){
			printf("%c", c);
			buf[i++] = c;
			c = fgetc(f);
		}
		printf("\n");
		buf[i] = '\0';
		current->string = buf;
		char* ptr;
		if((ptr = strstr(buf, "#include:")) != NULL){
			char newFileName[100];
			sscanf(buf,"#include:%99[^ =:]:", newFileName);
			printf("%s\n", newFileName);
			doubleLinkedList* tempList;
			tempList = includeFile(newFileName);

			previous->next = current;
			current->previous = previous;

			previous = tempList->tail;
			current->next = tempList->head;
			tempList->head->previous = current;
			list->tail = tempList->tail;
			free(tempList);
			current -> index = position++;
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