#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

#include "include/WorkDistribution.hpp"
#include "../../include/DataManipulationLib.hpp"

unsigned int countDirElements(std::string dirName) {
    DIR *dirPtr;
    struct dirent *direntPtr;
    errno = 0;
    int counter = 0;

    if ((dirPtr = opendir(dirName.c_str())) == NULL && errno != 0) {
        perror("countDirEnts");
        exit(3);
    }

    while ((direntPtr = readdir(dirPtr)))
        if (toString(direntPtr->d_name).compare(".") &&
            toString(direntPtr->d_name).compare(".."))
            counter++;
            
    closedir(dirPtr);
    
    return counter;
}

List<std::string> *distributeWorkLoad(std::string dirName, int numMonitors) {
    DIR *dirPtr;
    struct dirent *direntPtr;
    List<std::string> *monitorMap = new List<std::string>[numMonitors];
    List<std::string> subFolders;
    subFolders.flush();
    errno = 0;

    if ((dirPtr = opendir(dirName.c_str())) == NULL && errno != 0) {
        perror("distrWorkLoad");
        exit(3);
    }

    while ((direntPtr = readdir(dirPtr)))
        if (toString(direntPtr->d_name).compare(".") &&
            toString(direntPtr->d_name).compare(".."))
            subFolders.insertAscending(toString(direntPtr->d_name));

    int index = 0;
    // Simulate the Round-Robin algorithm alphabetically to distribute the subfolders
    for (unsigned int i = 0; i < subFolders.getSize(); i++) {
        index = i % numMonitors;
        monitorMap[index].insertLast(*subFolders.getNode(i));
    }

    closedir(dirPtr);

    return monitorMap;
}