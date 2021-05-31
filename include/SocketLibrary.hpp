#ifndef SOCKETLIBRARY_HPP
#define SOCKETLIBRARY_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

#define MIN_PORT 1024
#define MAX_PORT 65535
#define MAX_CONNECTIONS 5

int createSocket();
void bindSocket(int &sock, const sockaddr *sockPtr, socklen_t len);
void listenConnections(int sockfd, int maxConn);
int acceptConnection(int sockfd, sockaddr *addr, socklen_t *len);
void closeSocket(int sock);
void writeToSocket(int sockfd, void *message, size_t bufferSize);
void readFromSocket(int sockfd, void *message, size_t bufferSize);

#endif