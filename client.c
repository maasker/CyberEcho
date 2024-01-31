#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <limits.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>

#define SERVERPORT 9000 // Define server port
#define SOCKETERROR (-1) // Error code for socket operations
#define MAX_SIZE 50 // Maximum size for strings
#define MAX_BUFFER_SIZE 50 // Buffer size for messages
#define MAX_INPUT_SIZE 250 // Maximum input size for messages

typedef struct sockaddr_in SA_IN; // Structure for internet address
typedef struct sockaddr SA; // Generic structure for socket addresses

// Global variables
int login_accepted = 0;
int firstChat = 0;
int chat_socket = 0;
ssize_t amountReceived = 0;

// Condition and mutex for synchronization
pthread_cond_t condStatus;
pthread_mutex_t mutexLoginAccepted;
pthread_mutex_t mutexChatSocket;

// Function declarations
void startListeningAndPrintMessagesOnNewThread(int fd);
void readConsoleEntriesAndSendToServerOnNewThread(int socketFD);
void* listenAndPrint(void* p_socketFD);
int check(int exp, const char* msg);
void* readConsoleEntriesAndSendToServer(void* p_socketFD);
char** split_string(char* str, char* delim);
char* trimwhitespace(char* str);

int main() {
    int server_socket, addr_size;
    SA_IN server_addr;

    // Initialize condition and mutex
    pthread_cond_init(&condStatus, NULL);
    pthread_mutex_init(&mutexLoginAccepted, NULL);
    pthread_mutex_init(&mutexChatSocket, NULL);

    // Create socket and check for errors
    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create connection");

    printf("Socket successfully created..\n");

    // Set server address and port
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    // Connect to the server
    check(connect(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Connection with the server failed...\n");
    printf("Connected to the server..\n");

    // Start listening to messages from server on a new thread
    startListeningAndPrintMessagesOnNewThread(server_socket);

    // Start reading console entries and send to server on a new thread
    readConsoleEntriesAndSendToServerOnNewThread(server_socket);

    // Close resources
    close(server_socket);
    pthread_cond_destroy(&condStatus);
    pthread_mutex_destroy(&mutexLoginAccepted);
    pthread_mutex_destroy(&mutexChatSocket);

    return 0;
}

// Function to start a new thread for reading console entries and sending them to the server.
void readConsoleEntriesAndSendToServerOnNewThread(int socketFD) {
    pthread_t id;
    int* pclient = malloc(sizeof(int));
    *pclient = socketFD;
    pthread_create(&id, NULL, readConsoleEntriesAndSendToServer, pclient);

    if (pthread_join(id, NULL) != 0) {
        fprintf(stderr, "Error joining thread\n");
        return;
    }

    printf("Thread has completed.\n");
}


void* readConsoleEntriesAndSendToServer(void* p_socketFD)
{
    int socketFD = *((int*)p_socketFD);

    size_t lineSize = 0;

    while (true)
    {
        // send  login  request to server
        if (login_accpeted == 0)
        {
            char username[MAX_SIZE];
            char password[MAX_SIZE] = "password1";

            printf("Enter username: \n");
            scanf("%s", username);
            // printf("Enter password: ");
            // scanf("%s", password);

            if (strlen(username) == 0 && strlen(password) == 0)
                continue;

            char* auth_buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));
            memset(auth_buffer, '\0', sizeof(auth_buffer));

            sprintf(auth_buffer, "login|%s|%s", username, password);

            ssize_t amountWasSent = send(socketFD,
                auth_buffer,
                strlen(auth_buffer), 0);
            printf("auth message sent\n");

            free(auth_buffer);
        }
        else if (amountReceived > 0)
        {
            char* buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));

            if (firstChat == 1)
            {
                char line[250];
                memset(buffer, '\0', sizeof(buffer));
                memset(line, '\0', sizeof(line));

                printf("Enter Client SocketId: \n");
                scanf("%d", &chat_socket);

                while (getchar() != '\n')
                    ;

                printf("Enter message: \n");
                scanf("%250[^\n]", line);

                trimwhitespace(line);

                if (strcmp(line, "switch") == 0)
                {
                    pthread_mutex_lock(&mutexChatSocket);
                    chat_socket = 0;
                    firstChat = 1;
                    pthread_mutex_unlock(&mutexChatSocket);
                    continue;
                }

                int charCount = strlen(line);

                if (charCount == 0)
                    continue;

                if (strcmp(line, "exit") == 0)
                    break;

                sprintf(buffer, "newsend|%d|%s", chat_socket, line);
                printf("newsend message sent \n");
                ssize_t amountWasSent = send(socketFD,
                    buffer,
                    strlen(buffer), 0);
            }
            else
            {
                char line[250];
                memset(buffer, '\0', sizeof(buffer));
                memset(line, '\0', sizeof(line));

                // while (getchar() != '\n')
                //     ;
                fflush(stdin);
                printf("Enter message: \n");
                fgets(line, 250, stdin);

                int charCount = strlen(line);

                trimwhitespace(line);

                if (strcmp(line, "switch") == 0)
                {
                    pthread_mutex_lock(&mutexChatSocket);
                    chat_socket = 0;
                    firstChat = 1;
                    pthread_mutex_unlock(&mutexChatSocket);
                    continue;
                }

                if (charCount == 0)
                {
                    printf("message not sent\n");
                    continue;
                }

                if (strcmp(line, "exit") == 0)
                    break;

                sprintf(buffer, "send|%d|%s", chat_socket, line);
                printf("message sent\n");
                ssize_t amountWasSent = send(socketFD,
                    buffer,
                    strlen(buffer), 0);
            }

            free(buffer);
        }

        pthread_cond_wait(&condStatus, &mutexloginAccpeted);
    }

    printf("Exit\n");
    return NULL;
}

void startListeningAndPrintMessagesOnNewThread(int socketFD)
{
    pthread_t id;
    int* pclient = malloc(sizeof(int));
    *pclient = socketFD;
    pthread_create(&id, NULL, listenAndPrint, pclient);
}

void* listenAndPrint(void* p_socketFD)
{
    int socketFD = *((int*)p_socketFD);
    char delim[] = "|";
    char buffer[1024];

    while (true)
    {
        amountReceived = recv(socketFD, buffer, 1024, 0);

        login_accpeted = 1;

        if (amountReceived > 0)
        {
            buffer[amountReceived] = 0;
            login_accpeted = 1;

            // printf("Response was: \n%s\n ", buffer);
            char** tokens = split_string(buffer, delim);

            if (strcmp(tokens[0], "ok") == 0 && strcmp(tokens[1], "login") == 0)
            {
                pthread_mutex_lock(&mutexloginAccpeted);
                login_accpeted = 1;
                pthread_mutex_unlock(&mutexloginAccpeted);
                firstChat = 1;
                printf("Response was: \n%s\n", tokens[2]);
            }
            else if (strcmp(tokens[0], "error") == 0 && strcmp(tokens[1], "login") == 0)
            {
                pthread_mutex_lock(&mutexloginAccpeted);
                login_accpeted = 0;
                pthread_mutex_unlock(&mutexloginAccpeted);
                printf("Response was: \n%s\n", tokens[2]);
            }
            else if (strcmp(tokens[0], "error") == 0 && strcmp(tokens[1], "send") == 0)
            {
                firstChat = 1;
                printf("Response was: \n%s\n", tokens[2]);
            }
            else if (strcmp(tokens[0], "error") == 0 && strcmp(tokens[1], "newsend") == 0)
            {
                firstChat = 1;
                printf("Response was: \n%s\n", tokens[2]);
            }
            else if (strcmp(tokens[0], "ok") == 0 && strcmp(tokens[1], "send") == 0)
            {
                char socket_char[5];
                sprintf(socket_char, "%d", socketFD);
                int sender_socket = atoi(tokens[3]);

                if (chat_socket == sender_socket)
                {
                    pthread_mutex_lock(&mutexChatSocket);
                    chat_socket = atoi(tokens[3]);
                    firstChat = 0;
                    pthread_mutex_unlock(&mutexChatSocket);
                    system("clear");
                    printf("Response was: \n%s\n", tokens[2]);
                }
                else
                {
                    printf("**New Message From SocketId: %s\n", tokens[3]);
                }
            }
        }

        // if (amountReceived == 0)
        //     break;
        pthread_cond_signal(&condStatus);
    }
    printf("Exit");

    close(socketFD);
}

int check(int exp, const char* msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}

char** split_string(char* str, char* delim)
{
    char** tokens = NULL;
    int n = 0;
    char* token = strtok(str, delim);
    while (token != NULL)
    {
        tokens = realloc(tokens, sizeof(char*) * (n + 1));
        if (tokens == NULL)
        {
            printf("Memory allocation error\n");
            exit(1);
        }
        tokens[n] = strdup(token);
        n++;
        token = strtok(NULL, delim);
    }
    tokens = realloc(tokens, sizeof(char*) * (n + 1));
    if (tokens == NULL)
    {
        printf("Memory allocation error\n");
        exit(1);
    }
    tokens[n] = NULL;
    return tokens;
}

char* trimwhitespace(char* str)
{
    char* end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}