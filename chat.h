#ifndef CHAT_H
#define CHAT_H

#include <netinet/in.h>

// Message structure for storing chat messages
struct Message {
    char sender[50];         // Sender username
    char receiver[50];       // Receiver username
    char message[250];       // Chat message content
    struct Message *next;    // Pointer to next message in linked list
};

// User structure for storing user information
struct User {
    char username[50];             // Username
    char datetime[50];             // Date and time of user joining
    struct Message *chatHistory;   // Pointer to the head of user's chat history
    struct User *next;             // Pointer to next user in linked list
    struct sockaddr_in clientAddr; // Client address information
    int sockID;                    // Socket ID for user's connection
    int len;                       // Length of client address structure
};

// Function prototypes
void addMessage(struct Message **history, const char *username, const char *receiver, const char *message);
void addUser(struct User **head, const char *username, const char *datetime);
struct User *findUserByName(char username[]);
void setUserSocketId(struct User *head, char *username, int socketId);
struct User *findUserBySocketId(int socketId);
char *getUserChat(struct User *user, const char *receiverName);
char *getOnlineUserList(int currentClientSocketId);
void updateChatFile(const char *username, const char *receiver, const char *message);
void updateChatStatus(struct User *user, struct User *receiver_user, const char *message);

#endif // CHAT_H
