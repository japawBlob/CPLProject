#include "chw09.h"

char *command[] = {
    "LED ON",
    "LED OFF",
    "BUTTON?",
    "JOYSTICK?",
};

int main(int argc, char const *argv[]){
    printf("program started\n");
    const char* serialPort;
    if(argc < 2){
        serialPort = "/dev/ttyACM1";
    } else {
        serialPort = argv[1];
    }
 
    hSerial = open( serialPort, O_RDWR| O_NONBLOCK | O_NDELAY );
 
    struct termios o_tty;
    memset (&o_tty, 0, sizeof o_tty);
    tcgetattr (hSerial , &o_tty);
 
 
    // set raw input, 1 second timeout 
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
 
    // set the options 
    tcsetattr(hSerial, TCSANOW, &o_tty);

    pthread_mutex_init(&messageReady, NULL);
    pthread_mutex_lock(&messageReady);
    pthread_mutex_init(&messageLock, NULL);
    pthread_mutex_init(&endProgram, NULL);
    pthread_mutex_lock(&endProgram);
    pthread_mutex_init(&fileExecutionReady, NULL);
    pthread_mutex_init(&readyToJoinExecutinThread, NULL);
    pthread_mutex_lock(&readyToJoinExecutinThread);
    pthread_mutex_init(&needResponse, NULL);
    pthread_mutex_lock(&needResponse);
    pthread_mutex_init(&responseLock, NULL);
    pthread_mutex_lock(&responseLock);

    pthread_t communicationHandler;
    pthread_t fileExecutinHandler;

    pthread_create(&communicationHandler, NULL, communication, NULL);

    prom = 0;
    char in;
    int ledState = 0;
    while(1){
        scanf(" %c", &in);

        if (pthread_mutex_trylock(&readyToJoinExecutinThread) == 0){
            pthread_join(fileExecutinHandler, NULL);
            pthread_mutex_unlock(&fileExecutionReady);
            printf("Thread joined\n");
        }

        switch(in){
            case '1':
            {    
                pthread_mutex_lock(&messageLock);
                sprintf(glob_message, "%s", command[ledState]);
                pthread_mutex_unlock(&messageReady);
                ledState = ledState ? 0 : 1;
                break;
            }
            case '2':
            {
                pthread_mutex_lock(&messageLock);
                sprintf(glob_message, "%s", command[BUTTON]);
                pthread_mutex_unlock(&messageReady);
                break;
            }
            case '3':
            {
                pthread_mutex_lock(&messageLock);
                sprintf(glob_message, "%s", command[JOYSTICK]);
                pthread_mutex_unlock(&messageReady);
                break;
            }
            case '4':
            {
                char *fileName = (char*)malloc(sizeof(char)*255);
                scanf("%254s", fileName);
                if(pthread_mutex_trylock(&fileExecutionReady) == 0){
                    pthread_create(&fileExecutinHandler, NULL, executeCommandsFromFile, (void*)fileName);
                } else {
                    printf("I am sorry, file execution is already running, please wait until it finishes\n");
                }
                break;
            }
            case 'c':
            {
                char customCommand [200];
                scanf(" %[^\n]", customCommand);
                pthread_mutex_lock(&messageLock);
                sprintf(glob_message, "%s", customCommand);
                pthread_mutex_unlock(&messageReady);
                break;
            }
            case 'h':
            {
                printMenu();
                break;
            }
            case 'e':
            {
                pthread_mutex_unlock(&endProgram);
                goto exitWhile;
                break;
            }
            default :
            {
                fprintf(stderr, "Wrong option\n");
            }
        }
    }
    exitWhile:
    if(pthread_mutex_trylock(&fileExecutionReady) != 0){
        while (pthread_mutex_trylock(&readyToJoinExecutinThread) == 0){
            pthread_join(fileExecutinHandler, NULL);
            printf("Thread joined\n");
        }
    }
    
    pthread_join(communicationHandler, NULL);

    /*doubleLinkedList* mainList = includeFile("main.txt");
    node* current = mainList->head;
    while(current != NULL){
        printf("%i    %s\n", current->index, current->string);
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
    free(mainList);*/
    pthread_mutex_destroy(&messageReady);
    pthread_mutex_destroy(&messageLock);
    pthread_mutex_destroy(&endProgram);
    pthread_mutex_destroy(&fileExecutionReady);
    pthread_mutex_destroy(&readyToJoinExecutinThread);
    pthread_mutex_destroy(&needResponse);
    pthread_mutex_destroy(&responseLock);

    close(hSerial);
 
    return 0;
}