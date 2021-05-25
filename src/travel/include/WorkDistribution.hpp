#ifndef WORKDISTRIBUTION_HPP
#define WORKDISTRIBUTION_HPP

#include <iostream>

#include "../../../include/List.hpp"

unsigned int countDirElements(std::string dirName);
List<std::string> *distributeWorkLoad(std::string dirName, int numMonitors);

#endif