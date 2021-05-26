#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../../include/AppStandards.hpp"
#include "../../include/BloomFilter.hpp"
#include "../../include/DataManipulationLib.hpp"
#include "../../include/Date.hpp"
#include "../../include/HashTable.hpp"
#include "../../include/List.hpp"
#include "../../include/LogHistory.hpp"
#include "../../include/Messaging.hpp"
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

    // =========== Insert Record Info Start ===========

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

    // ============= Insert Record Info End =============

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

// Reads all the files in given fileList and imports the records
// Params: fileList, recordInfo, recordObjects, database,
// total Inconsistent, total Duplicates, total Records
void importFileRecords(List<string> fileList, recordInfo &recInfo,
                       recordObject &obj, appDataBase &db,
                       unsigned int &inc, unsigned int &dup, unsigned int &total) {
    std::ifstream inputFile;
    string line;
    bool recordOk = false, inserted = true;

    for (unsigned int file = 0; file < fileList.getSize(); file++) {
        // These counters are local for every file
        unsigned int incRecords = 0, dupRecords = 0, totalLocal = 0;
        inputFile.open(*fileList.getNode(file));
        if (!inputFile.is_open()) {
            std::cerr << OPEN_FAILED << *fileList.getNode(file) << std::endl;
            exit(-1);
        }

        while (std::getline(inputFile, line)) {
            totalLocal++; total++;
            // Split the current line in variables and do basic validation
            recordOk = testRecord(line, recInfo, obj, db);
            if (recordOk) {
                // Then insert the valid info into the appropriate structures of the app
                inserted = insertNewRecord(recInfo, obj, db);
                if (!inserted) {
                    std::cerr << DUPLICATE_RECORD << line << std::endl;
                    dupRecords++; dup++;
                }
            } else {
                incRecords++; inc++;
                std::cerr << INCONSISTENT_RECORD << line << std::endl;
                // Dump this record
                continue;
            }
        }

        inputFile.close();

        /* Uncomment below to show specific stats for each file */
        // std::cout << "\nDone reading '" << *fileList.getNode(file) << "'\n"
        // << "Excluded " << (incRecords + dupRecords)
        // << "/" << totalLocal << " records.\n"
        // << "Inconsistent records: " << std::setw(4) << incRecords << std::endl
        // << "Duplicate records:" << std::setw(8) << dupRecords << std::endl;
    }

    /* Uncomment below to show total stats for all files read */
    // std::cout << "\n==========================\n" << getpid() << " Completed insertion.\n"
    // << "Excluded " << (inc + dup)
    // << "/" << total << " records.\n"
    // << "Inconsistent records: " << std::setw(4) << inc << std::endl
    // << "Duplicate records:" << std::setw(8) << dup << std::endl;
}

// Sends all BloomFilters of known viruses to the server
void sendBloomFilters(appDataBase &db, int fdWrite, unsigned int bloomSize,
                      unsigned int bufferSize, unsigned int totalInc,
                      unsigned int totalDup, unsigned int totalRecs) {
    string line;
    char *buffer;
    /* Inform the server of the completion with the following formatted message: */
    /* [PID] [1/0](Success/Failure) [totalInc] [totalDupl] [totalRecs] [totalViruses] */
    line.assign(toString(getpid()) + " ");
    line.append(toString(1) + " ");
    line.append(toString(totalInc) + " ");
    line.append(toString(totalDup) + " ");
    line.append(toString(totalRecs) + " ");
    line.append(toString(db.virusList.getSize()));
    sendPackets(fdWrite, line.c_str(), line.length()+1, bufferSize);

    // Send all the bloom filters to the server
    for (unsigned int virus = 0; virus < db.virusList.getSize(); virus++) {
        // 1: virusName
        line.assign(db.virusList.getNode(virus)->getName());
        sendPackets(fdWrite, line.c_str(), line.length()+1, bufferSize);
        // 2: bloomFilter bitArray
        buffer = db.virusList.getNode(virus)->getBloom();
        sendPackets(fdWrite, buffer, bloomSize, bufferSize);
    }
}

int main(int argc, char *argv[]) {

    // =========== Input Arguments Validation ===========
    List<string> args;
    for (int i = 0; i < argc; i++)
        args.insertLast(toString(argv[i]));
    // args: 1) path/to/fifo1 2) path/to/fifo2
    if (!checkMonitorArgs(args)) die("monitor/input", -1);

    args.print();

    // ========== Variables ==========

    unsigned int port = myStoi(argv[2]);
    unsigned int numThreads = myStoi(argv[4]);
    unsigned int bufferSize = myStoi(argv[6]);
    unsigned int cBufferSize = myStoi(argv[8]);
    unsigned int bloomSize = myStoi(argv[10]);
    unsigned totalInc = 0, totalDup = 0, totalRecs = 0;
    string line;
    char *buffer;
    List<string> folders, fileList, newFileList, tempList;

    bool request = false;
    unsigned int acceptedReqs = 0, rejectedReqs = 0;

    // // argv[1] is the fifo client-X and argv[2] fifo server-X
    // int fdWrite = fifoOpenWrite(toString(argv[1]));
    // int fdRead = fifoOpenRead(toString(argv[2]));

    // // ========== Signal Handling ==========

    // struct sigaction sa = {0};
    // sa.sa_handler = &signalHandler;
    // sa.sa_flags = SA_RESTART;
    // sigemptyset(&sa.sa_mask);
    // if (sigaction(SIGINT, &sa, NULL) < 0) die("monitor/sigaction, 4");
    // if (sigaction(SIGQUIT, &sa, NULL) < 0) die("monitor/sigaction, 4");
    // if (sigaction(SIGUSR1, &sa, NULL) < 0) die("monitor/sigaction, 4");
    // if (sigaction(SIGUSR2, &sa, NULL) < 0) die("monitor/sigaction, 4");

    // // ========== Communication installation START ==========

    // // 1st message contains the bufferSize
    // fifoRead(fdRead, &bufferSize, sizeof(int));
    // // 2nd message contains the bloomSize
    // fifoRead(fdRead, &bloomSize, sizeof(int));
    // // 3rd message contains all the subfolders we need to read
    // buffer = receivePackets(fdRead, bufferSize);
    // line.assign(buffer);
    // delete[] buffer;
    // // Extract all the folders we need to read
    // splitLine(line, folders);
    // initFileList(folders, fileList);

    // // ========== Create and initialize app resources ==========
    // // Keeps all necessary info for a record
    // recordInfo recInfo;
    // // Contains all the required objects that are used as buffer to insert
    // // the info of a record into the appropriate data structure
    // // It's initialised with the desired bloomSize
    // recordObject obj(bloomSize);
    // // Contains all the data structrures that implement the app's database for the queries
    // appDataBase db;

    // /* Validate & import the input records into the application structure */
    // importFileRecords(fileList, recInfo, obj, db, totalInc, totalDup, totalRecs);

    // // Reply to the server
    // sendBloomFilters(db, fdWrite, bloomSize, bufferSize, totalInc, totalDup, totalRecs);

    // // ========== Communication installation END ==========

    // std::cout << MONITOR_STARTED(getpid());

    // // Pointers to handle all the structures and objects
    // Virus *virusPtr;
    // Person *personPtr;
    // Record *recordPtr;

    // // Variables for menu options
    // string command;
    // int option = -1;

    // // ========== Main application - Queries ==========

    // do {

    //     // Wait for a signal
    //     pause();
    //     // Detect SIGUSR1
    //     if (newFilesAdded) {
    //         // Reset the flag
    //         newFilesAdded = false;
    //         option = addRecords;
    //     }
    //     // Detect SIGUSR2
    //     else if (newCommand) {
    //         // Reset the flag
    //         newCommand = false;
    //         buffer = receivePackets(fdRead, bufferSize);
    //         line.assign(buffer);
    //         delete[] buffer;
    //         splitLine(line, args);
    //         if (isInt(args.getFirst())) {
    //             option = myStoi(args.getFirst());
    //             args.popFirst();
    //         }
    //     }
    //     // Detect SIGINT/SIGQUIT
    //     else if (shutDown) {
    //         shutDown = false;
    //         option = exitProgram;
    //     }
    //     else continue;

    //     switch (option) {

    //         case travelRequest:

    //             // Server is requesting in format: [citizenID] [virus]
    //             recInfo.idStr.assign(args.getFirst());
    //             obj.record.setID(myStoi(recInfo.idStr));
    //             recInfo.virusName.assign(args.getLast());
    //             obj.virus.setName(recInfo.virusName);
    //             virusPtr = db.virusList.search(obj.virus);
    //             if (virusPtr) {
    //                 recordPtr = virusPtr->searchVaccinatedList(obj.record);
    //                 if (recordPtr) {
    //                     request = true;
    //                     obj.date1 = recordPtr->getDate();
    //                 }
    //             }

    //             // Update local counters
    //             request ? acceptedReqs++ : rejectedReqs++;
                
    //             // Send the result to the back server
    //             request ? line.assign("YES " + toString(obj.date1)) : line.assign("NO");
    //             sendPackets(fdWrite, line.c_str(), line.length()+1, bufferSize);
    //             break;

    //         case travelStats:
    //             // Nothing to do here
    //             break;

    //         case addRecords:

    //             initFileList(folders, tempList);
    //             // Keep only the new files
    //             for (unsigned int file = 0; file < tempList.getSize(); file++)
    //                 if (!fileList.search(*tempList.getNode(file)))
    //                     newFileList.insertAscending(*tempList.getNode(file));

    //             if (newFileList.empty()) {
    //                 sendPackets(fdWrite, NOT_FOUND, sizeof(NOT_FOUND), bufferSize);
    //                 break;
    //             } else sendPackets(fdWrite, UPDATE, sizeof(UPDATE), bufferSize);

    //             // Update the database
    //             importFileRecords(newFileList, recInfo, obj, db, totalInc, totalDup, totalRecs);

    //             // Reply to the server
    //             sendBloomFilters(db, fdWrite, bloomSize, bufferSize, totalInc, totalDup, totalRecs);

    //             // Update the fileList
    //             for (unsigned int file = 0; file < newFileList.getSize(); file++)
    //                 fileList.insertAscending(*newFileList.getNode(file));
    //             newFileList.flush();
    //             break;

    //         case searchStatus:

    //             obj.person.setID(myStoi(args.getFirst()));
    //             personPtr = db.citizenRegistry.search(args.getFirst(), obj.person);
    //             if (!personPtr) {
    //                 line.assign(NOT_FOUND);
    //                 sendPackets(fdWrite, line.c_str(), line.length()+1, bufferSize);
    //                 break;
    //             }

    //             // Reply format: [ID] [NAME] [SURNAME] [COUNTRY] [AGE] [VACCINATION-LIST]
    //             line.assign(args.getFirst() + " " +
    //                         personPtr->getFirstName() + " " +
    //                         personPtr->getLastName() + " " +
    //                         personPtr->getCountry().getName() + " " +
    //                         toString(personPtr->getAge()));

    //             obj.record.setID(personPtr->ID());

    //             for (unsigned int virus = 0; virus < db.virusList.getSize(); virus++) {
    //                 virusPtr = db.virusList.getNode(virus);
    //                 line.append(" " + virusPtr->getName() + " ");
    //                 recordPtr = NULL;
    //                 if (virusPtr->checkBloom(args.getFirst()))
    //                     recordPtr = virusPtr->searchVaccinatedList(obj.record);
    //                 recordPtr ? line.append(recordPtr->getDate()) : line.append("NO");
    //             }

    //             sendPackets(fdWrite, line.c_str(), line.length()+1, bufferSize);
    //             break;

    //         default:
    //             break;
    //     }
    // } while (option);

    // for (unsigned int i = 0; i < db.countryList.getSize(); i++)
    //     tempList.insertAscending(db.countryList.getNode(i)->getName());

    // // Save the request statistics in log files
    // writeLogFile(tempList, toString(LOG_FILES), PERMS, acceptedReqs, rejectedReqs);

    // std::cout << MONITOR_STOPPED(getpid());

    // fifoClose(fdRead);
    // fifoClose(fdWrite);

    return 0;
}