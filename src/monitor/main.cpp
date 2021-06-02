#include <dirent.h>
#include <pthread.h>

#include <fstream>
#include <iomanip>

#include "../../include/AppStandards.hpp"
#include "../../include/BloomFilter.hpp"
#include "../../include/DataManipulationLib.hpp"
#include "../../include/Date.hpp"
#include "../../include/HashTable.hpp"
#include "../../include/List.hpp"
#include "../../include/LogHistory.hpp"
#include "../../include/Messaging.hpp"
#include "../../include/Queue.hpp"
#include "../../include/SignalLibrary.hpp"
#include "../../include/SkipList.hpp"
#include "../../include/SocketLibrary.hpp"
#include "include/Country.hpp"
#include "include/Person.hpp"
#include "include/Record.hpp"
#include "include/Virus.hpp"
#include "include/VirusCountryEntry.hpp"

// Variables used as temporary buffer to store a record's arguments
struct recordInfo {
    string idStr, firstName, lastName, countryName,
           ageStr, virusName, status, dateVaccinated;
};

// Upon constructing this object we set up the virus object to have the desired
// bloomSize. Since all objects of this struct are meant to be used as buffer
// variables to initialize all application objects, every virus that will be
// created, will be a copy of this object at its initial size and bitArray
// values, with the difference to its name value. Thus, every different virus
// differs only by name, and has the same size and initial bitArray status as
// all the others. Symmetrically, every person is copied by this Person object
// we just change this objects desired info before doing so. Same goes for each
// country, record and vCountryEntry objects that will be created.
struct recordObject {
    Virus virus;
    Person person;
    Country country;
    Record record;
    VirusCountryEntry vCountryEntry;
    Date date1, date2;
    // Constructor to set up the desired bloom size for every virus bloom filter
    // Note that we will NOT insert any citizen in this virus filter!!
    recordObject(unsigned int bloomSize) : virus(bloomSize) {}
};

// Basic data structrures that implement the database of the app
struct appDataBase {
    // A table that hold all the citizen information
    HashTable<Person> citizenRegistry;
    // A list with all known viruses
    List<Virus> virusList;
    // A list with all known countries
    List<Country> countryList;
    // A table with entries that associate virus statistics for every country
    HashTable<VirusCountryEntry> entriesTable;
    appDataBase(unsigned int pTableSize = CITIZEN_REGISTRY_SIZE,
        unsigned int entryTableSize = VIRUS_COUNTRY_ENTRIES)
        : citizenRegistry(pTableSize), entriesTable(entryTableSize) {}
};

// Statistics for files read
unsigned totalInc = 0, totalDup = 0, totalRecs = 0;

struct circularBuffer {
    Queue<string> cBufNodes;
    pthread_mutex_t dbLock;
    pthread_mutex_t bufLock;
    pthread_cond_t condNonEmpty;
    pthread_cond_t condNonFull;
    volatile bool end;
    circularBuffer(unsigned int size)
    : cBufNodes(size), dbLock(PTHREAD_MUTEX_INITIALIZER), bufLock(PTHREAD_MUTEX_INITIALIZER),
    condNonEmpty(PTHREAD_COND_INITIALIZER), condNonFull(PTHREAD_COND_INITIALIZER), end(false) {}
    ~circularBuffer() {
        pthread_mutex_destroy(&dbLock);
        pthread_mutex_destroy(&bufLock);
        pthread_cond_destroy(&condNonEmpty);
        pthread_cond_destroy(&condNonEmpty);
    }
};

struct prodInfo {
    List<string> fileList;
    circularBuffer *cBufPtr;
    prodInfo(List<string> files, circularBuffer *cBuf) : fileList(files), cBufPtr(cBuf) {}
};

struct consInfo {
    recordInfo *recPtr;
    recordObject *objPtr;
    appDataBase *dbPtr;
    circularBuffer *cBufPtr;
    consInfo(recordInfo *r, recordObject *o, appDataBase *a, circularBuffer *c)
    : recPtr(r), objPtr(o), dbPtr(a), cBufPtr(c) {}
};

// Reads and validates a record line while also splitting it.
// It stores the splitted results into given string variables
static bool testRecord(string &line, recordInfo &recInfo, recordObject &obj, appDataBase &db) {
    List<string> args;
    Date date;
    // Push the arguments in the argument list
    splitLine(line, args);
    // Check if the record had enough information
    if (args.getSize() != 7 && args.getSize() != 8) return false;

    // Retrieve the arguments from the argument list
    recInfo.idStr.assign(*args.getNode(0));
    recInfo.firstName.assign(*args.getNode(1));
    recInfo.lastName.assign(*args.getNode(2));
    recInfo.countryName.assign(*args.getNode(3));
    recInfo.ageStr.assign(*args.getNode(4));
    recInfo.virusName.assign(*args.getNode(5));
    recInfo.status.assign(*args.getNode(6));

    if (!isInt(recInfo.idStr)) return false;
    // A negative ID is not valid
    if (myStoi(recInfo.idStr) < 0) return false;
    // Check for non alpha characters in firstName
    if (containsNonAlpha(recInfo.firstName)) return false;
    // Check for non alpha characters in lastName
    if (containsNonAlpha(recInfo.lastName)) return false;
    // Check for non alpha characters in countryName
    if (containsNonAlpha(recInfo.countryName)) return false;
    if (!isInt(recInfo.ageStr)) return false;
    // A negative age is not valid
    if (myStoi(recInfo.ageStr) < 0) return false;
    // The status will either be YES or NO
    if (recInfo.status.compare("YES") && recInfo.status.compare("NO")) return false;
    // If there's no date after YES, the record is faulty
    if (!recInfo.status.compare("YES") && args.getSize() != 8) return false;
    // If there's a date after NO, the record is faulty
    if (!recInfo.status.compare("NO") && args.getSize() != 7) return false;
    // Read the date for status == YES or set it to invalid for NO
    if (!recInfo.status.compare("YES"))
        recInfo.dateVaccinated.assign(*args.getNode(7));
    else recInfo.dateVaccinated.clear();
    // Set the object Date properly for further validation
    date.set(recInfo.dateVaccinated);
    // Validate dates ONLY for records with vaccination status = YES
    // because for NO we have set the dates to be 0-0-0 by default
    // which would cause every NO record to fail this test
    if (!date.valid() && !recInfo.status.compare("YES")) return false;

    // ================== Person Identification ==================
    obj.country.setName(recInfo.countryName);
    Country *countryPtr = db.countryList.search(obj.country);
    if (countryPtr) {
        // If that's NOT the first record from this country, check out this person's info
        obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                       recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
        Person *personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
        if (personPtr)  // If this ID is already in the registry, validate all the rest person's info
            if (!personPtr->isIdentical(obj.person)) return false;
    }

    // If we made it till here, the record is valid...
    return true;
}

// Returns true on successfull import and false if the record was a duplicate
bool insertNewRecord(recordInfo &recInfo, recordObject &obj, appDataBase &db) {
    // ========== Duplicates check - Start ==========

    // Set up the new record and virus information
    obj.record.set(myStoi(recInfo.idStr), recInfo.dateVaccinated);
    obj.virus.setName(recInfo.virusName);

    // Look for the appropriate virus
    Virus *virusPtr = db.virusList.search(obj.virus);
    if (!virusPtr) {  // If that's the first time we see this virus, insert it as new
        db.virusList.insertAscending(obj.virus);
        // Search again for it, as it should be in the list now
        virusPtr = db.virusList.search(obj.virus);
        // Initialize this virus filter by copying the virus prototype (required for bloomSize)
        virusPtr->initializeBloom(obj.virus);
    } else {  // In this case we need to check if this is a duplicate record
        // If the person already has a record in EITHER of the two
        // skip lists, or the bloom filter, then this is a duplicate record
        if (virusPtr->checkBloom(recInfo.idStr))
            // Attempt to save some time by asking the filter first and not the skip list
            if (virusPtr->searchVaccinatedList(obj.record)) return false;
        if (virusPtr->searchNonVaccinatedList(myStoi(recInfo.idStr))) return false;
    }

    // =========== Duplicates check - End ===========

    // =========== Insert Record Info - Start ===========

    // Since the record passed the duplication check we can now insert it
    if (!recInfo.status.compare("YES")) {
        // For positive records we insert both, in the bloom filter
        // and the vaccinated skip list of the current virus
        virusPtr->insertBloom(recInfo.idStr);
        virusPtr->insertVaccinatedList(obj.record);
    } else {
        // Insert in non-vaccinated skip-list.
        // We're not seting up a record this time,
        // as there's no point to store a date.
        // So we just pass the citizen's ID
        virusPtr->insertNonVaccinatedList(myStoi(recInfo.idStr));
    }

    // ======= Store country person and virus information =======

    // Look for the appropriate country
    obj.country.setName(recInfo.countryName);
    Country *countryPtr = db.countryList.search(obj.country);
    if (!countryPtr) {
        // If that's the first citizen, create the new country
        db.countryList.insertAscending(obj.country);
        // Search again for it, as it should be in the list now
        countryPtr = db.countryList.search(obj.country);
    }

    // Set up the person object
    obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                   recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
    Person *personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
    // Insert the person information in the registry only if he isn't already in
    if (!personPtr) db.citizenRegistry.insert(recInfo.idStr, obj.person);

    // Insert the new virus-country entry if its NOT already stored
    VirusCountryEntry *entryPtr;
    // Set up the new entry
    obj.vCountryEntry.set(virusPtr, countryPtr);
    // The key of each entry is virusName + countryName
    entryPtr = db.entriesTable.search(recInfo.virusName + recInfo.countryName, obj.vCountryEntry);
    if (!entryPtr) {
        // Insert it in the table
        db.entriesTable.insert(recInfo.virusName + recInfo.countryName, obj.vCountryEntry);
        // And search for it again, as it must be in now
        entryPtr = db.entriesTable.search(recInfo.virusName + recInfo.countryName, obj.vCountryEntry);
    }
    // Finally, having ensured entryPtr != NULL we can make the appropriate accounting
    // for the vaccination statistics in the corresponding virus-country table
    entryPtr->registerPerson(myStoi(recInfo.ageStr), recInfo.status);

    // ============= Insert Record Info - End =============

    // If we made it till here, the record was imported successfully...
    return true;
}

// Opens and reads all folders in given list and stores found files in fileList
void initFileList(List<string> folders, List<string> &fileList) {
    DIR *dirPtr;
    struct dirent *direntPtr;
    string filePath;
    errno = 0;
    fileList.flush();

    // Open every subdirectory
    for (unsigned int i = 0; i < folders.getSize(); i++) {
        if ((dirPtr = opendir((*folders.getNode(i)).c_str())) == NULL && errno != 0)
            die("monitor/initFileList", 25);

        while ((direntPtr = readdir(dirPtr))) {
            filePath.assign(*folders.getNode(i));
            if (toString(direntPtr->d_name).compare(".") &&
                toString(direntPtr->d_name).compare("..")) {
                filePath.append(toString(direntPtr->d_name));
                fileList.insertAscending(filePath);
            }
        }
        closedir(dirPtr);
    }
}

// Reads the given file and imports its records in the main database
void importFileRecords(string file, recordInfo &recInfo, recordObject &obj, appDataBase &db) {
    std::ifstream inputFile;
    string line;
    bool recordOk = false, inserted = true;

    // These counters are local for the current file
    unsigned int incRecords = 0, dupRecords = 0, totalLocal = 0;
    inputFile.open(file);
    if (!inputFile.is_open()) {
        std::cerr << OPEN_FAILED << file << std::endl;
        exit(-1);
    }

    while (std::getline(inputFile, line)) {
        totalLocal++; totalRecs++;
        // Split the current line in variables and do basic validation
        recordOk = testRecord(line, recInfo, obj, db);
        if (recordOk) {
            // Then insert the valid info into the appropriate structures of the app
            inserted = insertNewRecord(recInfo, obj, db);
            if (!inserted) {
                std::cerr << DUPLICATE_RECORD << line << std::endl;
                dupRecords++; totalDup++;
            }
        } else {
            incRecords++; totalInc++;
            std::cerr << INCONSISTENT_RECORD << line << std::endl;
            // Dump this record
            continue;
        }
    }

    inputFile.close();

    /* Uncomment below to show stats for current file */
    // std::cout << "\nDone reading '" << file << "'\n"
    // << "Excluded " << (incRecords + dupRecords)
    // << "/" << totalLocal << " records.\n"
    // << "Inconsistent records: " << std::setw(4) << incRecords << std::endl
    // << "Duplicate records:" << std::setw(8) << dupRecords << std::endl;
}

// Sends all BloomFilters of known viruses to the travelClient
void sendBloomFilters(appDataBase &db, int sockfd, unsigned int bloomSize, unsigned int bufferSize) {
    string line;
    char *buffer;
    /* Inform the client of the completion with the following formatted message: */
    /* [PID] [1/0](Success/Failure) [totalInc] [totalDup] [totalRecs] [totalViruses] */
    line.assign(toString(getpid()) + " ");
    line.append(toString(1) + " ");
    line.append(toString(totalInc) + " ");
    line.append(toString(totalDup) + " ");
    line.append(toString(totalRecs) + " ");
    line.append(toString(db.virusList.getSize()));
    sendPackets(sockfd, line.c_str(), line.length()+1, bufferSize);

    // Send all the bloom filters to the server
    for (unsigned int virus = 0; virus < db.virusList.getSize(); virus++) {
        // 1: virusName
        line.assign(db.virusList.getNode(virus)->getName());
        sendPackets(sockfd, line.c_str(), line.length()+1, bufferSize);
        // 2: bloomFilter bitArray
        buffer = db.virusList.getNode(virus)->getBloom();
        sendPackets(sockfd, buffer, bloomSize, bufferSize);
    }
}

// ==================== Threads ====================

// Main thread is the producer and pool contains the consumers for the circular buffer

void place(string file, circularBuffer &cBuffer) {
    pthread_mutex_lock(&cBuffer.bufLock);
    while (cBuffer.cBufNodes.full())
        pthread_cond_wait(&cBuffer.condNonFull, &cBuffer.bufLock);
    cBuffer.cBufNodes.enqueue(file);
    // std::cout << "Placed " << file << " in cyclic buffer\n";
    pthread_mutex_unlock(&cBuffer.bufLock);
}

string obtain(circularBuffer &cBuffer) {
    string file;
    pthread_mutex_lock(&cBuffer.bufLock);
    while (cBuffer.cBufNodes.empty())
        pthread_cond_wait(&cBuffer.condNonEmpty, &cBuffer.bufLock);
    cBuffer.cBufNodes.dequeue(file);
    // std::cout << "Removed " << file << " from cyclic buffer\n";
    pthread_mutex_unlock(&cBuffer.bufLock);
    return file;
}

// producer function is not void *f(void *) type, because
// it's supposed to be called only by the main thread
void producer(void *arg) {
    prodInfo *info = (prodInfo *)arg;
    while (!info->fileList.empty()) {
        place(info->fileList.getFirst(), *info->cBufPtr);
        info->fileList.popFirst();
        pthread_cond_signal(&info->cBufPtr->condNonEmpty);
    }
    info->cBufPtr->end = true;
}

void *consumer(void *arg) {
    consInfo *info = (consInfo *)arg;
    string file;
    while(!info->cBufPtr->end || !info->cBufPtr->cBufNodes.empty()) {
        file = obtain(*info->cBufPtr);
        pthread_cond_signal(&info->cBufPtr->condNonFull);
        // Lock the mutex to read the file because we need
        // the data insertion in the main database to be atomic
        pthread_mutex_lock(&info->cBufPtr->dbLock);
        importFileRecords(file, *info->recPtr, *info->objPtr, *info->dbPtr);
        pthread_mutex_unlock(&info->cBufPtr->dbLock);
    }
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    // =========== Input Arguments Validation ===========
    List<string> args;
    for (int i = 0; i < argc; i++)
        args.insertLast(toString(argv[i]));
    if (!checkMonitorArgs(args)) die("monitor/input", -1);

    // ========== Variables ==========

    /* Regular variables */
    int sock = 0, newsock = 0;
    uint16_t port = myStoi(argv[2]);
    unsigned int numThreads = myStoi(argv[4]);
    unsigned int bufferSize = myStoi(argv[6]);
    unsigned int cBufferSize = myStoi(argv[8]);
    unsigned int bloomSize = myStoi(argv[10]);
    unsigned int acceptedReqs = 0, rejectedReqs = 0;
    string line;
    char *buffer = NULL;
    bool request = false;
    List<string> folders, fileList, newFileList, tempList;

    /* Networking variables */
    struct sockaddr_in monitor = {'\0'}, travel = {'\0'};
    socklen_t travelLen = sizeof(travel);
    struct sockaddr *monitorPtr = (struct sockaddr *)&monitor;
    struct sockaddr *travelPtr = (struct sockaddr *)&travel;
    struct hostent *remHost;

    /* Database structs */
    // Keeps all necessary info for a record
    recordInfo recInfo;
    // Contains all the required objects that are used as buffer to insert
    // the info of a record into the appropriate data structure
    // It's initialised with the desired bloomSize
    recordObject obj(bloomSize);
    // Contains all the data structrures that implement the app's database for the queries
    appDataBase db;
    // Simulates the cyclic buffer
    circularBuffer cBuffer(cBufferSize);

    // ========== Initialize app resources ==========

    // Store all the folders we need to read
    for (int i = 11; i < argc; i++) folders.insertLast(toString(argv[i]));
    initFileList(folders, fileList);

    // Prepare thread arguments
    prodInfo prodArgs(fileList, &cBuffer);
    consInfo consArgs(&recInfo, &obj, &db, &cBuffer);

    // Create and start numThread consumers
    pthread_t pool_t[numThreads];
    for (unsigned int t = 0; t < numThreads; t++)
        if (pthread_create(&pool_t[t], NULL, consumer, &consArgs))
            die("pthread_create", -10);

    // Call the producer function after we create the consumers,
    // as the main thread will be busy while producing information.
    producer(&prodArgs);

    // Wait for all consumers to finish
    for (unsigned int t = 0; t < numThreads; t++)
        if (pthread_join(pool_t[t], NULL))
            die("monitor/pthread_join", -11);
    // Reset the flag in case there are more files in the future
    cBuffer.end = false;

    /* Uncomment below to show total stats for all files read */
    // std::cout << "\n==========================\n" << getpid() << " Completed insertion.\n"
    // << "Excluded " << (totalInc + totalDup)
    // << "/" << totalRecs << " records.\n"
    // << "Inconsistent records: " << std::setw(4) << totalInc << std::endl
    // << "Duplicate records:" << std::setw(8) << totalDup << std::endl;

    std::cout << MONITOR_STARTED(getpid());

    // ========== Communication installation START ==========

    // Create socket
    sock = createSocket();
    monitor.sin_family = AF_INET;
    monitor.sin_addr.s_addr = htonl(INADDR_ANY);
    monitor.sin_port = htons(port);
    // Bind socket to address
    bindSocket(sock, monitorPtr, sizeof(monitor));
    // Listen for connections
    listenConnections(sock, MAX_CONNECTIONS);
    std::cout << LISTENING_TO(getpid(), port);

    newsock = acceptConnection(sock, travelPtr, &travelLen);
    if ((remHost = gethostbyaddr((const void *)&travel.sin_addr,
        sizeof(travel.sin_addr), travel.sin_family)) == NULL) {
        herror("monitor/gethostbyaddr");
        exit(-4);
    }
    std::cout << ACCEPTED_CONN(getpid(), remHost->h_name);

    // Reply to the travelClient
    sendBloomFilters(db, newsock, bloomSize, bufferSize);

    // ========== Communication installation END ==========

    // Pointers to handle all the structures and objects
    Virus *virusPtr;
    Person *personPtr;
    Record *recordPtr;

    // Variables for menu options
    string command;
    int option = -1;

    // ========== Main application - Queries ==========

    do {

        // Read and parse the command
        buffer = receivePackets(newsock, bufferSize);
        line.assign(buffer);
        delete[] buffer;
        splitLine(line, args);
        if (isInt(args.getFirst())) {
            option = myStoi(args.getFirst());
            args.popFirst();
        }

        switch (option) {

            case travelRequest:

                if (args.getFirst().compare(REQUEST)) {
                    // This is an update message from client, either ["ACCEPTED"] or ["REJECTED"]
                    !args.getFirst().compare(ACCEPTED) ? acceptedReqs++ : rejectedReqs++;
                } else {
                    // This is a request from client in format ["REQUEST"] [citizenID] [virus]
                    args.popFirst();
                    recInfo.idStr.assign(args.getFirst());
                    obj.record.setID(myStoi(recInfo.idStr));
                    recInfo.virusName.assign(args.getLast());
                    obj.virus.setName(recInfo.virusName);
                    virusPtr = db.virusList.search(obj.virus);
                    if (virusPtr) {
                        recordPtr = virusPtr->searchVaccinatedList(obj.record);
                        if (recordPtr) {
                            request = true;
                            obj.date1 = recordPtr->getDate();
                        }
                    }
                    // Send the result to the back travelClient
                    request ? line.assign("YES " + toString(obj.date1)) : line.assign("NO");
                    sendPackets(newsock, line.c_str(), line.length()+1, bufferSize);
                }
                break;

            case travelStats:
                // Nothing to do here
                break;

            case addRecords:

                initFileList(folders, tempList);
                // Keep only the new files
                for (unsigned int file = 0; file < tempList.getSize(); file++)
                    if (!fileList.search(*tempList.getNode(file)))
                        newFileList.insertAscending(*tempList.getNode(file));

                if (newFileList.empty()) {
                    sendPackets(newsock, NOT_FOUND, sizeof(NOT_FOUND), bufferSize);
                    break;
                } else sendPackets(newsock, UPDATE, sizeof(UPDATE), bufferSize);

                prodArgs.fileList = newFileList;
                // Start threads to update the database
                for (unsigned int t = 0; t < numThreads; t++)
                    if (pthread_create(&pool_t[t], NULL, consumer, &consArgs))
                        die("pthread_create", -10);

                // Call the producer function after we create the consumers,
                // as the main thread will be busy while producing information.
                producer(&prodArgs);

                // Wait for all consumers to finish
                for (unsigned int t = 0; t < numThreads; t++)
                    if (pthread_join(pool_t[t], NULL))
                        die("monitor/pthread_join", -11);
                // Reset the flag in case there are more files in the future
                cBuffer.end = false;

                // Reply to the travelClient
                sendBloomFilters(db, newsock, bloomSize, bufferSize);

                // Update the fileList
                for (unsigned int file = 0; file < newFileList.getSize(); file++)
                    fileList.insertAscending(*newFileList.getNode(file));
                newFileList.flush();
                break;

            case searchStatus:

                obj.person.setID(myStoi(args.getFirst()));
                personPtr = db.citizenRegistry.search(args.getFirst(), obj.person);
                if (!personPtr) {
                    line.assign(NOT_FOUND);
                    sendPackets(newsock, line.c_str(), line.length()+1, bufferSize);
                    break;
                }

                // Reply format: [ID] [NAME] [SURNAME] [COUNTRY] [AGE] [VACCINATION-LIST]
                line.assign(args.getFirst() + " " +
                            personPtr->getFirstName() + " " +
                            personPtr->getLastName() + " " +
                            personPtr->getCountry().getName() + " " +
                            toString(personPtr->getAge()));

                obj.record.setID(personPtr->ID());

                for (unsigned int virus = 0; virus < db.virusList.getSize(); virus++) {
                    virusPtr = db.virusList.getNode(virus);
                    line.append(" " + virusPtr->getName() + " ");
                    recordPtr = NULL;
                    if (virusPtr->checkBloom(args.getFirst()))
                        recordPtr = virusPtr->searchVaccinatedList(obj.record);
                    recordPtr ? line.append(recordPtr->getDate()) : line.append("NO");
                }

                sendPackets(newsock, line.c_str(), line.length()+1, bufferSize);
                break;

            default:
                break;
        }
    } while (option);

    for (unsigned int i = 0; i < db.countryList.getSize(); i++)
        tempList.insertAscending(db.countryList.getNode(i)->getName());

    // Save the request statistics in log files
    writeLogFile(tempList, toString(LOGS_PATH), PERMS, acceptedReqs, rejectedReqs);

    std::cout << MONITOR_STOPPED(getpid());

    closeSocket(sock);
    closeSocket(newsock);

    return EXIT_SUCCESS;

}