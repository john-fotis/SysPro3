#include <sys/stat.h>
#include <errno.h>
#include <fstream>
#include <unistd.h>

#include "../../include/DataManipulationLib.hpp"
#include "../../include/LogHistory.hpp"

void writeLogFile(List<std::string> countryList, std::string dirName,
                  mode_t permissions, unsigned int acc, unsigned int rej) {
    std::string fileName(dirName + "log_file."), line;
    fileName.append(toString(getpid()));
    struct stat buf;
    if (stat(dirName.c_str(), &buf))
        if (mkdir(dirName.c_str(), permissions) < 0 && errno != EEXIST) {
            perror("writeLogFile/mkdir");
            exit(30);
        }
    
    if (!stat(fileName.c_str(), &buf)) {
        if (remove(fileName.c_str()) < 0) {
            perror("writeLogFile/remove");
            exit(31);
        }
    }

    while(!countryList.empty()) {
        line.append(countryList.getFirst() + '\n');
        countryList.popFirst();
    }
    line.append("TOTAL TRAVEL REQUESTS " + toString(acc + rej) + '\n' +
                "ACCEPTED " + toString(acc) + '\n' +
                "REJECTED " + toString(rej) + '\n');

    std::ofstream logFile(fileName);
    logFile << line;        
}