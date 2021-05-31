#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/AppStandards.hpp"
#include "../../include/SocketLibrary.hpp"

int createSocket() {
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) die("socket/create", 11);
    return sock;
}

void bindSocket(int &sock, const sockaddr *sockPtr, socklen_t len) {
    if (bind(sock, sockPtr, len) < 0) die("socket/bind", 12);
}

void listenConnections(int sockfd, int maxConn) {
    if(listen(sockfd, maxConn) < 0) die("socket/listen", 13);
}

int acceptConnection(int sockfd, sockaddr *addr, socklen_t *len) {
    int newsock = 0;
    if ((newsock = accept(sockfd, addr, len)) < 0) die("socket/accept", 14);
    return newsock;
}

void closeSocket(int sock) {
    if (close(sock) < 0) die("socket/close", 15);
}

void writeToSocket(int sockfd, void *message, size_t bufferSize) {
    if (send(sockfd, message, bufferSize, 0) < 0) {
        perror("socket/write");
        exit(16);
    }
}

void readFromSocket(int sockfd, void *message, size_t bufferSize) {
    if (recv(sockfd, message, bufferSize, 0) < 0) {
        perror("socket/read");
        exit(17);
    }
}