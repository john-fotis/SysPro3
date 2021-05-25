#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/AppStandards.hpp"
#include "../../include/SocketLibrary.hpp"

int createSocket() {
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) die("createSocket", 11);
    return sock;
}