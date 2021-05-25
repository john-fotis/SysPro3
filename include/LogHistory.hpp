#ifndef LOGHISTORY_HPP
#define LOGHISTORY_HPP

#include <iostream>

#include "List.hpp"

void writeLogFile(List<std::string> countryList, std::string dirName,
                  mode_t permissions, unsigned int acc, unsigned int rej);

#endif