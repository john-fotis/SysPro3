#include <wait.h>

#include <csignal>
#include <cstdlib>
#include <ctime>

#include "../../include/AppStandards.hpp"
#include "../../include/BloomFilter.hpp"
#include "../../include/DataManipulationLib.hpp"
#include "../../include/Date.hpp"
#include "../../include/HashTable.hpp"
#include "../../include/List.hpp"
#include "../../include/LogHistory.hpp"
#include "../../include/Messaging.hpp"
#include "../../include/SocketLibrary.hpp"
#include "include/MonitorInfo.hpp"
#include "include/Request.hpp"
#include "include/RequestRegistry.hpp"
#include "include/VirusRegistry.hpp"
#include "include/WorkDistribution.hpp"

volatile sig_atomic_t alarmTimeOut = false;

void alarmHandler(int signo) {
    if (signo == SIGALRM) alarmTimeOut = true;
    // Cancel any other alarms
    alarm(0);
    return;
}

// Receives and stores all viruses and bloomfilters from a Monitor
void getMonitorInfo(List<VirusRegistry> &virusList, MonitorInfo *monitorPtr,
                    unsigned int bloomSize, unsigned int bufferSize) {
    VirusRegistry virus(bloomSize);
    VirusRegistry *virusPtr = NULL;
    string line;
    List<string> args;
    char *buffer;

    // Format: [PID] [STATUS] [INCONSISTENT] [DUPLICATES] [TOTAL READ] [NUM VIRUSES]
    // This message can be used to track errors in initialization phase
    buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
    line.assign(buffer);
    delete[] buffer;
    splitLine(line, args);

    if (args.empty() || myStoi(*args.getNode(1)) == false)
        std::cout << MONITOR_ERROR(monitorPtr->PID());

    unsigned int numViruses = myStoi(args.getLast());
    
    for (unsigned int v = 0; v < numViruses; v++) {
        // Read current virus name
        buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
        virus.setName(toString(buffer));
        delete[] buffer;

        // Read the bloom filter array of the current virus
        buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
        // And store it in the virus object
        virus.initializeBloom(buffer, bloomSize);

        virusPtr = virusList.search(virus);
        if (!virusPtr) {
            // If virus is not in the list, insert and search again
            virusList.insertAscending(virus);
            virusPtr = virusList.search(virus);
            // Now initialise its filter from the virus object
            virusPtr->initializeBloom(virus);
        } else {
            virusPtr->mergeBloom(buffer, bloomSize);
        }
        delete[] buffer;
    }
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    // =========== Input Arguments Validation ===========
    List<string> args;
    for (int i = 0; i < argc; i++) args.insertLast(argv[i]);
    if (!checkTravelArgs(args)) die("travel/input", -1);

    // =========== Variables Start ===========

    unsigned int numMonitors = myStoi(argv[2]);
    unsigned int bufferSize = myStoi(argv[4]);
    unsigned int cBufferSize = myStoi(argv[6]);
    unsigned int bloomSize = myStoi(argv[8]);
    string inputDir(argv[10]);
    if (inputDir.back() != '/') inputDir.append("/");
    unsigned int numThreads = myStoi(argv[12]);
    extern unsigned int TIME_OUT;
    if (argc == 15) TIME_OUT = myStoi(argv[14]);

    /* Regular variables */
    pid_t pid = 0;
    int status = 0, exitStatus = 0;
    string line, country;
    bool reqAnswer = false, found = false;
    char *buffer = NULL, hostName[NI_MAXHOST], symbolicIP[100];
    unsigned int acceptedReqs = 0, rejectedReqs = 0;
    unsigned int localAccRecs = 0, localRejRecs = 0;

    /* Objects */
    MonitorInfo monitor;
    VirusRegistry virus(bloomSize);
    Request request;
    RequestRegistry registry;
    Date date1, date2;

    /* Pointers */
    MonitorInfo *monitorPtr = NULL;
    string *monCountryPtr = NULL;
    VirusRegistry *virusPtr = NULL;
    Request *reqPtr = NULL;
    RequestRegistry *registryPtr = NULL;

    /* Structs */
    List<MonitorInfo> monitorList;
    // A map of $numMonitor lists with country names that they are responsible for
    List<string> *monitorWorkMap;
    // A list of known countries based on sub-directories read
    List<string> countryList;
    // A list of known viruses based on requests
    List<VirusRegistry> virusList;
    // This entity stores all requests made to the travelClient
    // distinguished by the destination country
    List<RequestRegistry> registryList;

    /* Networking variables */
    struct sockaddr *serverPtr = NULL;
    hostent *remHost = NULL;
    struct in_addr **addr_list = NULL;

    // =========== Variables End ===========

    // =========== Signal handling ===========
    struct sigaction sa = {0};
    sa.sa_handler = &alarmHandler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0) die("travel/sigaction", -9);

    std::cout << SERVER_STARTING;

    // If the number of country subfolders is less than the number of monitors
    // reduce the number of monitors to the max number of available subfolders
    if (countDirElements(inputDir) < numMonitors) {
        numMonitors = countDirElements(inputDir);
        std::cerr << REDUCE_MONITORS(numMonitors)
        << NOT_ENOUGH_RESOURCES(inputDir) << std::endl;
    }

    // Distribute the available countries subfolders to the monitors
    monitorWorkMap = distributeWorkLoad(inputDir, numMonitors);
    
    // ========== Communication installation START ==========

    // Find the servers' information
    if (gethostname(hostName, sizeof(hostName)) < 0) {
        herror("travel/gethostname");
        exit(-2);
    }
    
    if ((remHost = gethostbyname(hostName)) == NULL) {
        herror("travel/gethostbyname");
        exit(-3);
    }

    std::cout << SEARCHING_MONITOR(remHost->h_name);
    if (remHost->h_aliases != NULL)
        for (int i = 0; remHost->h_aliases[i] != NULL; i++)
            std::cout << ALTERNATIVE_NAME(remHost->h_name);

    addr_list = (struct in_addr **)remHost->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        sprintf(symbolicIP, "%s", inet_ntoa(*addr_list[i]));
        std::cout << RESOLVED_HOST(remHost->h_name, symbolicIP);
    }

    // Set up all the monitors' info except their PIDs
    for (unsigned int mon = 0; mon < numMonitors; mon++) {
        monitor.setSocket(createSocket());
        monitor.setFamily(AF_INET);
        monitor.setAddress(remHost);
        // Pick a random port between [1024-65535]
        monitor.setPort(htons((rand() % (MAX_PORT - MIN_PORT + 1)) + MIN_PORT));
        monitor.setCountries(monitorWorkMap[mon]);
        monitorList.insertLast(monitor);
    }

    // Create all the Monitor processes
    for (unsigned int mon = 0; mon < numMonitors; mon++) {
        if ((pid = fork()) < 0) die("travel/fork", 1);
        else if (pid == 0) {
            monitorPtr = monitorList.getNode(mon);
            unsigned int argc = 11 + monitorPtr->countriesNumber() + 1;
            char *program = strdup("./monitorServer");
            char **argv = new char*[argc];
            argv[0] = strdup("./monitorServer");
            argv[1] = strdup("-p");
            argv[2] = strdup(toString(monitorPtr->getPort()).c_str());
            argv[3] = strdup("-t");
            argv[4] = strdup(toString(numThreads).c_str());
            argv[5] = strdup("-b");
            argv[6] = strdup(toString(bufferSize).c_str());
            argv[7] = strdup("-c");
            argv[8] = strdup(toString(cBufferSize).c_str());
            argv[9] = strdup("-s");
            argv[10] = strdup(toString(bloomSize).c_str());
            unsigned int argPos = 11;
            for (unsigned int path = 0; path < monitorPtr->countriesNumber(); path++) {
                argv[argPos] = strdup((inputDir+monitorPtr->getNthCountry(path)+"/").c_str());
                argPos++;
            }
            argv[argPos] = NULL;
            // Free up the memory before calling exec
            delete[] monitorWorkMap;
            virus.~VirusRegistry();
            monitor.~MonitorInfo();
            args.~List();
            monitorList.~List();
            execvp(program, argv);
            // If the process continues here, exec has failed
            die("travel/exec", 2);
        } else monitorList.getNode(mon)->setPID(pid);
    }

    for (unsigned int mon = 0; mon < numMonitors; mon++) {
        monitorPtr = monitorList.getNode(mon);
        serverPtr = (struct sockaddr *)&monitorPtr->getServer();
        std::cout << ATTEMPTING_CONN(remHost->h_name, monitorPtr->getPort());

        // The alarm protects from connection timing-out
        alarm(TIME_OUT);
        do {
            errno = 0;
            if (connect(monitorPtr->getSocket(), serverPtr, monitorPtr->getSockLen()) < 0)
                if (errno != ECONNREFUSED) die("travel/socket/connect", 3);
            // ECONNREFUSED happens only when there is no-one to listen
            // to this socket, but since we know we have to correct socket,
            // this indicates that the server is not up yet. So we can keep
            // attempting to connect until we succeed.
        } while (errno && !alarmTimeOut);
        
        if (alarmTimeOut) {
            std::cerr << CONNECTION_TIMED_OUT;
            // Terminate all monitors
            for (unsigned int mon = 0; mon < numMonitors; mon++)
                kill(monitorList.getNode(mon)->PID(), SIGKILL);
            for (unsigned int mon = 0; mon < numMonitors; mon++)
                waitpid(monitorList.getNode(mon)->PID(), NULL, 0);
            exit(EXIT_FAILURE);
        }
        // Cancel pending alarms after successfull connection
        alarm(0);
    }

    for (unsigned int mon = 0; mon < numMonitors; mon++)
        getMonitorInfo(virusList, monitorList.getNode(mon), bloomSize, bufferSize);

    std::cout << SERVER_STARTED;

    // ========== Communication installation END ==========

    // Variables for menu options
    string command;
    int option = -1;

    // ========== Main application - Queries ==========

    do {

        option = -1;
        while (option == -1) {
            std::cout << SPOILER;
            getline(std::cin, line);
            if (std::cin.eof() || std::cin.fail()) std::cin.clear();

            if (!line.empty()) {
                splitLine(line, args);
                command = args.getFirst();
                args.popFirst();
                option = getOptions(command);
            }
        }

        switch (option) {

            case travelRequest:

                // Argument checking...
                if (args.getSize() != 5) { std::cerr << ARGS_NUMBER << std::endl; break; }
                if (!isInt(args.getFirst())) { std::cerr << INV_ID; break; }
                date1.set(*args.getNode(1));
                if (!date1.valid()) { std::cerr << INV_DATE; break; }
                // Locate which monitor is responsible for this country
                country.assign(*args.getNode(2));
                for (unsigned int mon = 0; mon < monitorList.getSize(); mon++) {
                    monitorPtr = monitorList.getNode(mon);
                    monCountryPtr = monitorWorkMap[mon].search(country);
                    if (monCountryPtr) break;
                }
                if(!monCountryPtr) { std::cerr << NO_COUNTRY; break; }
                virus.setName(args.getLast());
                if (!(virusPtr = virusList.search(virus))) { std::cerr << NO_VIRUS; break; }
                // Save the countryTo argument for statistics purpose
                country.assign(*args.getNode(3));

                // All good with arguments, now execute the query
                reqAnswer = virusPtr->checkBloom(args.getFirst());
                if (!reqAnswer) std::cout << COUT_REQ_REJECTED;
                else {
                    reqAnswer = false;
                    // Promote the request to monitorPtr
                    // Message format: [QUERY-CODE] ["REQUEST"] [ID] [COUNTRY] [VIRUS]
                    line.assign(toString(travelRequest) + " ");
                    line.append(toString(REQUEST) + " ");
                    line.append(args.getFirst() + " ");
                    line.append(args.getLast());

                    sendPackets(monitorPtr->getSocket(), line.c_str(), line.length()+1, bufferSize);
                    buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
                    line.assign(buffer);
                    delete[] buffer;
                    splitLine(line, args);

                    if (!args.getFirst().compare("NO")) std::cout << COUT_REQ_REJECTED;
                    else if (!args.getFirst().compare("YES")) {
                        date2.set(args.getLast());
                        if (date1.daysDifference(date2) < 0)
                            std::cout << COUT_REQ_REJECTED;
                        else if (date1.daysDifference(date2) >= (6*DAYS_PER_MONTH))
                            std::cout << COUT_REQ_REJECTED2;
                        else {
                            reqAnswer = true;
                            std::cout << COUT_REQ_ACCEPTED;
                        }
                    } else { std::cerr << UNKNOWN_ERROR; break; }
                }

                // Update counters
                reqAnswer ? acceptedReqs++ : rejectedReqs++;

                // Save the request data
                request.set(virusPtr, reqAnswer, date1);
                registry.setCountry(country);
                registryPtr = registryList.search(registry);
                if (!registryPtr) {
                    registryList.insertAscending(registry);
                    registryPtr = registryList.search(registry);
                }
                registryPtr->addRequest(request);

                // Time to inform the monitor of the request status
                line.assign(toString(travelRequest) + " ");
                reqAnswer ? line.append(ACCEPTED) : line.append(REJECTED);
                sendPackets(monitorPtr->getSocket(), line.c_str(), line.length()+1, bufferSize);
                break;

            case travelStats:

                // Argument checking...
                if (registryList.empty()) { std::cout << NO_DATA_IN_DB; break; }
                if (args.getSize() != 3 && args.getSize() != 4) { std::cerr << ARGS_NUMBER; break; }
                virus.setName(args.getFirst());
                if (!(virusPtr = virusList.search(virus))) { std::cerr << NO_VIRUS; break; }
                date1.set(*args.getNode(1));
                date2.set(*args.getNode(2));
                if (!date1.valid() || !date2.valid()) { std::cerr << INV_DATE; break; }
                if (date1 > date2) { std::cerr << DATE_IN_ORDER; break; }
                if (args.getSize() == 4) {
                    country.assign(args.getLast());
                    registry.setCountry(country);
                    registryPtr = registryList.search(registry);
                    if(!registryPtr) { std::cerr << NO_COUNTRY_DATA; break; }
                }

                // All good with arguments, now execute the query
                if (args.getSize() == 3) {
                    // Country argument was not given
                    for (unsigned int country = 0; country < registryList.getSize(); country++) {
                        localAccRecs = localRejRecs = 0;
                        // Locate the destination country in the list
                        registryPtr = registryList.getNode(country);
                        // And check all its requests
                        for (unsigned int req = 0; req < registryPtr->getRequestsNum(); req++) {
                            reqPtr = registryPtr->getReqNode(req);
                            if (!reqPtr->getVirus().getName().compare(virusPtr->getName()) &&
                                reqPtr->getDate() >= date1 &&
                                reqPtr->getDate() <= date2)
                                reqPtr->getStatus() ? localAccRecs++ : localRejRecs++;
                        }
                        // Print statistics
                        std::cout << std::endl << registryList.getNode(country)->getCountry() << " "
                        << STATISTICS << " " << virusPtr->getName() << std::endl
                        << TOTAL_REQUESTS << " " << (localAccRecs + localRejRecs) << std::endl
                        << ACCEPTED << " " << localAccRecs << std::endl
                        << REJECTED << " " << localRejRecs << std::endl;
                    }
                } else {
                    // Country argument was given
                    localAccRecs = localRejRecs = 0;
                    // Locate the destination country in the list
                    registry.setCountry(country);
                    registryPtr = registryList.search(registry);
                    // And check all its requests
                    for (unsigned int req = 0; req < registryPtr->getRequestsNum(); req++) {
                        reqPtr = registryPtr->getReqNode(req);
                        if (!reqPtr->getVirus().getName().compare(virusPtr->getName()) &&
                            reqPtr->getDate() >= date1 &&
                            reqPtr->getDate() <= date2)
                            reqPtr->getStatus() ? localAccRecs++ : localRejRecs++;
                    }
                    // Print statistics
                    std::cout << std::endl << country << " "
                    << STATISTICS << " " << virusPtr->getName() << std::endl
                    << TOTAL_REQUESTS << " " << (localAccRecs + localRejRecs) << std::endl
                    << ACCEPTED << " " << localAccRecs << std::endl
                    << REJECTED << " " << localRejRecs << std::endl;
                }
                break;

            case addRecords:
                
                if (args.getSize() != 1) { std::cerr << ARGS_NUMBER; break; }
                // Locate which monitor is responsible for this country
                country.assign(args.getFirst());
                for (unsigned int mon = 0; mon < monitorList.getSize(); mon++) {
                    monitorPtr = monitorList.getNode(mon);
                    monCountryPtr = monitorWorkMap[mon].search(country);
                    if (monCountryPtr) break;
                }
                if(!monCountryPtr) { std::cerr << NO_COUNTRY; break; }

                // Notify the responsible monitor
                line.assign(toString(addRecords));
                sendPackets(monitorPtr->getSocket(), line.c_str(), line.length()+1, bufferSize);

                // Check if the monitor actually found new records
                buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
                line.assign(buffer);
                delete[] buffer;
                if (line.compare(UPDATE)) { std::cerr << NO_NEW_FILES; break; }

                // Receive the updated bloomfilters from the monitor
                getMonitorInfo(virusList, monitorPtr, bloomSize, bufferSize);
                std::cout << DATABASE_UPDATED;
                break;

            case searchStatus:
                
                if (args.getSize() != 1) { std::cerr << ARGS_NUMBER; break; }
                if (!isInt(args.getFirst())) { std::cerr << INV_ID; break; }

                found = false;
                // Notify all monitors
                line.assign(toString(searchStatus) + " " + args.getFirst());
                for (unsigned int mon = 0; mon < monitorList.getSize(); mon++)
                    sendPackets(monitorList.getNode(mon)->getSocket(), line.c_str(), line.length()+1, bufferSize);

                // They will either reply with ["404"] (not found) or the user data
                for (unsigned int mon = 0; mon < monitorList.getSize(); mon++) {
                    monitorPtr = monitorList.getNode(mon);
                    buffer = receivePackets(monitorPtr->getSocket(), bufferSize);
                    line.assign(buffer);
                    delete[] buffer;

                    if (!line.compare(NOT_FOUND)) continue;
                    
                    found = true;
                    // User data format: [ID] [NAME] [SURNAME] [COUNTRY] [AGE] [VACCINATION-LIST]
                    splitLine(line, args);
                    // Print data
                    for (unsigned int i = 0; i < 4; i++) {
                        std::cout << args.getFirst() << " ";
                        args.popFirst();
                    }
                    std::cout << std::endl << "AGE " << args.getFirst() << std::endl;
                    args.popFirst();
                    // The remaining data is like: VIRUS1, NO, VIRUS2, 1-1-2021, VIRUS3, NO...
                    while (!args.empty()) {
                        std::cout << args.getFirst() << " ";
                        args.popFirst();
                        args.getFirst().compare("NO") ?
                        std::cout << VACCINATED << args.getFirst() : std::cout << NOT_VACCINATED;
                        std::cout << std::endl;
                        args.popFirst();
                    }
                }
                if (!found) std::cerr << USER_NOT_FOUND;
                break;

            case help:
                printOptions();
                break;

            default:
                break;
        }

    } while (option);
  
    // =========== Shut down ===========

    std::cout << SERVER_STOPPING;

    // Retrieve all the countries that participated in the simulation
    for (unsigned int mon = 0; mon < numMonitors; mon++)
        for (unsigned int c = 0; c < monitorWorkMap[mon].getSize(); c++)
            countryList.insertAscending(*monitorWorkMap[mon].getNode(c));

    // Save the request statistics in log files
    writeLogFile(countryList, toString(LOGS_PATH), PERMS, acceptedReqs, rejectedReqs);

    // Terminate all monitors
    line.assign(toString(exitProgram));
    for (unsigned int mon = 0; mon < monitorList.getSize(); mon++)
        sendPackets(monitorList.getNode(mon)->getSocket(), line.c_str(), line.length()+1, bufferSize);

    // Wait for all Monitors to finish
    for (unsigned int mon = 0; mon < monitorList.getSize(); mon++)
        waitpid(monitorList.getNode(mon)->PID(), NULL, 0);

    for (unsigned int mon = 0; mon < monitorList.getSize(); mon++)
        closeSocket(monitorList.getNode(mon)->getSocket());

    std::cout << SERVER_STOPPED;
    std::cout << LOG_FILES_SAVED(LOGS_PATH);

    delete[] monitorWorkMap;

    if ((exitStatus = WEXITSTATUS(status)))
        std::cout << EXIT_CODE_FROM(getpid(), exitStatus) << std::endl;
        
    return EXIT_SUCCESS;

}