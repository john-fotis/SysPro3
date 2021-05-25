#ifndef APPSTANDARDS_HPP
#define APPSTANDARDS_HPP

#include "List.hpp"
#include <string>

typedef std::string string;

// Server parameters
#define PERMS 0775
#define FIFO_PATH "bin/"
#define LOG_FILES "logs/"
#define SERVER_FIFO "server-"
#define CLIENT_FIFO "client-"

// Monitor parameters
#define CITIZEN_REGISTRY_SIZE 1000
#define VIRUS_COUNTRY_ENTRIES 100

// System messages
#define INPUT_SERVER "\n./travelMonitorClient –m numMonitors -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads\n"
#define INPUT_CLIENT "\n./monitorServer -p port -t numThreads -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom path1 path2 ... pathn\n"
#define SERVER_STARTING "\nStarting the server...\n"
#define SERVER_STARTED "\nThe server is up.\n"
#define SERVER_STOPPING "\nStopping the server...\n"
#define SERVER_STOPPED "\nThe server is down.\n"
#define REDUCE_MONITORS(NUM) "\nHad to reduce the number of Monitors to " << NUM
#define NOT_ENOUGH_RESOURCES(DIR) " because of insufficient number of sub-directories in " << DIR
#define MONITOR_STARTED(PID) "Monitor " << PID << " is up.\n"
#define MONITOR_STOPPED(PID) "Monitor " << PID << " is down.\n"
#define MONITOR_REPLACE(OLD, NEW) "Replacing Monitor " << OLD << " with " << NEW << "...\n"
#define MONITOR_ERROR(PID) "\nAn error occurred with monitor #" << PID
#define SPOILER "\nType /help to display the available options or /exit to exit the application.\n\n"
#define LOG_FILES_SAVED(PATH) "\nlog-files have been stored in " << PATH << "\n\n"
#define EXIT_CODE_FROM(PID, CODE) "Exit status from " << PID << " was " << CODE

// Query messages
#define COUT_REQ_REJECTED "\nREQUEST REJECTED - YOU ARE NOT VACCINATED\n"
#define COUT_REQ_REJECTED2 "\nREQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n"
#define COUT_REQ_ACCEPTED "\nREQUEST ACCEPTED - HAPPY TRAVELS\n"
#define STATISTICS "STATISTICS"
#define TOTAL_REQUESTS "TOTAL REQUESTS"
#define TOTAL_ACCEPTED "TOTAL ACCEPTED"
#define TOTAL_REJECTED "TOTAL REJECTED"
#define DATABASE_UPDATED "\nTHE DATABASE HAS BEEN UPDATED\n"
#define VACCINATED "VACCINATED ON "
#define NOT_VACCINATED "NOT YET VACCINATED"

// Error messages
#define ARGS_NUMBER "\nINVALID NUMBER OF ARGUMENTS\n"
#define INV_ID "\nINVALID CITIZEN ID\n"
#define INV_DATE "\nINVALID DATE GIVEN\n"
#define DATE_IN_ORDER "\ndate1 SHOULD BE OLDER THAN date2\n"
#define NO_DATA "\nTHERE IS NO DATA YET\n"
#define NO_COUNTRY "\nNO SUCH COUNTRY FOUND\n"
#define NO_COUNTRY_DATA "\nTHERE IS NO DATA FOR THIS COUNTRY YET\n"
#define NO_VIRUS "\nNO SUCH VIRUS FOUND\n"
#define NO_NEW_FILES "\nNO NEW FILES FOUND\n"
#define USER_NOT_FOUND "\nUSER NOT FOUND IN DATABASE\n"
#define UNKNOWN_ERROR "\nSOMETHING WENT WRONG...\n"
#define OPEN_FAILED "COULDN'T OPEN "
#define DUPLICATE_RECORD "DUPLICATE RECORD: "
#define INCONSISTENT_RECORD "ERROR IN RECORD: "

// Pipe-message types
#define REQUEST "REQUEST"
#define ACCEPTED "ACCEPTED"
#define REJECTED "REJECTED"
#define UPDATE "UPDATE"
#define NOT_FOUND "404"

// Server error codes
enum serverErrors {
    travelArgsNum = 1,
    monitorIdentifier = 2,
    monitorNumber = 3,
    bufferIdentifier = 4,
    bufferSize = 5,
    cBufferIdentifier = 6,
    cBufferSize = 7,
    bloomIdentifier = 8,
    bloomSize = 9,
    directoryIdentifier = 10,
    directoryNotFound = 11,
    threadIdentifier = 12,
    numOfThreads = 13
};

// Client error codes
enum clientErrors {
    monitorArgsNum = 1,
    portIdentifier = 2,
    portNumber = 3,
    mThreadIdentifier = 4,
    mNumOfThreads = 5,
    mBufferIdentifier = 6,
    mBufferSize = 7,
    mCBufferIdentifier = 8,
    mCBufferSize = 9,
    mBloomIdentifier = 10,
    mBloomSize = 11,
    pathNotFound = 12
};

// Main menu option codes
enum menuOptions {
    exitProgram = 0,
    travelRequest = 1,
    travelStats = 2,
    addRecords = 3,
    searchStatus = 4,
    help = 5
};

bool checkTravelArgs(List<std::string> &args);
bool checkMonitorArgs(List<std::string> &args);
// Returns -1 on invalid option or number of selected option in enumeration
int getOptions(std::string input);
void printOptions();
void die(const char *err, int code);

#endif