#include <iostream>
#include <sys/stat.h>

#include "../../include/AppStandards.hpp"
#include "../../include/DataManipulationLib.hpp"

// Connection time-out seconds
unsigned int TIME_OUT = 10;

bool checkTravelArgs(List<std::string> &args) {
    unsigned int errorNumber = 0;
    struct stat inputDirName;

    if (args.getSize() < 13 || args.getSize() > 15) errorNumber = travelArgsNum;
    else {
        if (args.getNode(1)->compare("-m")) errorNumber = monitorIdentifier;
        if (isInt(*args.getNode(2))) {
            if (myStoi(*args.getNode(2)) < 1) errorNumber = monitorNumber;
        } else errorNumber = monitorNumber;
        if (args.getNode(3)->compare("-b")) errorNumber = bufferIdentifier;
        if (isInt(*args.getNode(4))) {
            if (myStoi(*args.getNode(4)) < 1) errorNumber = bufferSize;
        } else errorNumber = bufferSize;
        if (args.getNode(5)->compare("-c")) errorNumber = cBuffIdentifier;
        if (isInt(*args.getNode(6))) {
            if (myStoi(*args.getNode(6)) < 1) errorNumber = cBuffSize;
        } else errorNumber = cBuffSize;
        if (args.getNode(7)->compare("-s")) errorNumber = bloomIdentifier;
        if (isInt(*args.getNode(8))) {   
            if (myStoi(*args.getNode(8)) < 1) errorNumber = bloomSize;
        } else errorNumber = bloomSize;
        if (args.getNode(9)->compare("-i")) errorNumber = directoryIdentifier;
        if ((stat(args.getNode(10)->c_str(), &inputDirName))) errorNumber = directoryNotFound;
        if (args.getNode(11)->compare("-t")) errorNumber = threadIdentifier;
        if (isInt(*args.getNode(12))) {
            if (myStoi(*args.getNode(12)) < 1) errorNumber = numOfThreads;
        } else errorNumber = numOfThreads;
        if (args.getSize() == 15) {
            if (args.getNode(13)->compare("-o")) errorNumber = timeoutIdentifier;
            if (isInt((*args.getNode(14)))) {
                if (myStoi(*args.getNode(14)) < 1) errorNumber = timeoutValue;
            } else errorNumber = timeoutValue;
        }
    }

    if (errorNumber) {
        if (errorNumber == travelArgsNum) std::cerr << ARGS_NUMBER;
        else if (errorNumber == monitorIdentifier) std::cerr << "Invalid Monitor number identifier.\n";
        else if (errorNumber == monitorNumber) std::cerr << "Invalid number of Monitors.\n";
        else if (errorNumber == bufferIdentifier) std::cerr << "Invalid socket-buffer size identifier.\n";
        else if (errorNumber == bufferSize) std::cerr << "Invalid socket-buffer size.\n";
        else if (errorNumber == cBuffIdentifier) std::cerr << "Invalid cyclic-buffer size identifier.\n";
        else if (errorNumber == cBuffSize) std::cerr << "Invalid cyclic-buffer size.\n";
        else if (errorNumber == bloomIdentifier) std::cerr << "Invalid bloom filter identifier.\n";
        else if (errorNumber == bloomSize) std::cerr << "Invalid bloom filter size.\n";
        else if (errorNumber == directoryIdentifier) std::cerr << "Invalid input directory identifier.\n";
        else if (errorNumber == directoryNotFound) std::cerr << "Input directory not found.\n";
        else if (errorNumber == threadIdentifier) std::cerr << "Invalid thread number identifier.\n";
        else if (errorNumber == numOfThreads) std::cerr << "Invalid number of threads.\n";
        else if (errorNumber == timeoutIdentifier) std::cerr << "Invalid time-out identifier.\n";
        else if (errorNumber == timeoutValue) std::cerr << "Invalid time-out value.\n";
        std::cout << "Input should be like: " << INPUT_TRAVEL;
        return false;
    }

    return true;
}

bool checkMonitorArgs(List<std::string> &args) {
    unsigned int errorNumber = 0;
    struct stat inputFileName;

    if (args.getSize() < 12) errorNumber = monitorArgsNum;
    else {
        if (args.getNode(1)->compare("-p")) errorNumber = portIdentifier;
        if (isInt(*args.getNode(2)))
            if (myStoi(*args.getNode(2)) < 1) errorNumber = portNumber;
        if (args.getNode(3)->compare("-t")) errorNumber = mThreadIdentifier;
        if (isInt(*args.getNode(4)))
            if (myStoi(*args.getNode(4)) < 1) errorNumber = mNumOfThreads;
        if (args.getNode(5)->compare("-b")) errorNumber = mBufferIdentifier;
        if (isInt(*args.getNode(6)))
            if (myStoi(*args.getNode(6)) < 1) errorNumber = mBufferSize;
        if (args.getNode(7)->compare("-c")) errorNumber = mCBufferIdentifier;
        if (isInt(*args.getNode(8)))
            if (myStoi(*args.getNode(8)) < 1) errorNumber = mCBufferSize;
        if (args.getNode(9)->compare("-s")) errorNumber = mBloomIdentifier;
        if (isInt(*args.getNode(10)))
            if (myStoi(*args.getNode(10)) < 1) errorNumber = mBloomSize;
        for (unsigned int i = 11; i < args.getSize(); i++)
            if ((stat(args.getNode(i)->c_str(), &inputFileName))) errorNumber = pathNotFound;
    }

    if (errorNumber) {
        if (errorNumber == monitorArgsNum) std::cerr << ARGS_NUMBER;
        else if (errorNumber == portIdentifier) std::cerr << "Invalid port identifier\n";
        else if (errorNumber == portNumber) std::cerr << "Invalid port number\n";
        else if (errorNumber == mBufferIdentifier) std::cerr << "Invalid socket-buffer identifier\n";
        else if (errorNumber == mBufferSize) std::cerr << "Invalid socket-buffer size\n";
        else if (errorNumber == mCBufferIdentifier) std::cerr << "Invalid cyclic-buffer identifier\n";
        else if (errorNumber == mCBufferSize) std::cerr << "Invalid cyclic-buffer size\n";
        else if (errorNumber == mBloomIdentifier) std::cerr << "Invalid bloom filter identifier\n";
        else if (errorNumber == mBloomSize) std::cerr << "Invalid bloom filter size\n";
        else if (errorNumber == pathNotFound) std::cerr << "Invalid path given\n";
        std::cout << "Input should be like: " << INPUT_MONITOR;
        return false;
    }

    return true;
}

int getOptions (std::string input) {
    if (!input.compare("/exit")) return exitProgram;
    if (!input.compare("/travelRequest")) return travelRequest;
    if (!input.compare("/travelStats")) return travelStats;
    if (!input.compare("/addVaccinationRecords")) return addRecords;
    if (!input.compare("/searchVaccinationStatus")) return searchStatus;
    if (!input.compare("/help")) return help;
    return -1;
}

void printOptions() {
    std::cout << "\nOptions:\n" \
    "=============================================================\n" \
    "/travelRequest citizenID date countryFrom countryTo virusName\n" \
    "/travelStats virusName date1 date2 [country]\n" \
    "/addVaccinationRecords country\n" \
    "/searchVaccinationStatus citizenID\n" \
    "/exit\n" \
    "=============================================================\n";
}

void die(const char *err, int code) {
    perror(err);
    exit(code);
}