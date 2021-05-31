#include <cstring>

#include "../../include/Messaging.hpp"
#include "../../include/SocketLibrary.hpp"

void sendPackets(int fdWrite, const char *data, unsigned int msgSize, unsigned int buffSize) {
    if (!data) return;
    unsigned int numOfPackets = (msgSize / buffSize) + 1;
    unsigned int padding = 0;

    if (msgSize % buffSize) padding = buffSize - (msgSize % buffSize);
    if (!padding) numOfPackets--;

    // Copy the original message
    char message[numOfPackets * buffSize + 1] = {'\0'};
    memcpy(message, data, msgSize);

    // Fill the padding with termination characters
    for (unsigned int i = 0; i < padding; i++)
        message[(numOfPackets * buffSize) - padding + i] = '\0';

    // Send the number of packets
    writeToSocket(fdWrite, &numOfPackets, sizeof(int));
    // Send the padding of the last packet
    writeToSocket(fdWrite, &padding, sizeof(int));

    char packet[buffSize + 1] = {'\0'};
    // Send the data in packets
    for (unsigned int i = 0; i < numOfPackets; i++) {
        memcpy(packet, message + (i * buffSize), buffSize);
        writeToSocket(fdWrite, packet, buffSize);
    }
}

// Returns char pointer to the read message
char *receivePackets(int fdRead, unsigned int buffSize) {
    unsigned int numOfPackets = 0, padding = 0;
    
    // Read the number of packets
    readFromSocket(fdRead, &numOfPackets, sizeof(int));
    // Read the padding of the last packet
    readFromSocket(fdRead, &padding, sizeof(int));

    // The initial buffer will hold the message along with the padding
    char message[numOfPackets * buffSize + 1] = {0};

    char packet[buffSize + 1] = {'\0'};
    // Read the actual data
    for (unsigned int i = 0; i < numOfPackets; i++) {
        readFromSocket(fdRead, packet, buffSize);
        memcpy(message + (i * buffSize), packet, buffSize);
    }
    
    // Now we cut off the padding and get the original message
    char *data = new char[(numOfPackets * buffSize) - padding];
    memcpy(data, message, (numOfPackets * buffSize) - padding);
    return data;
}