#ifndef MESSAGING_HPP
#define MESSAGING_HPP

void sendPackets(int fdWrite, const char *data, unsigned int msgSize, unsigned int buffSize);
char *receivePackets(int fdRead, unsigned int buffSize);

#endif