#ifndef STRINGLIBRARY_HPP
#define STRINGLIBRARY_HPP

#include <iostream>
#include <sstream>

#include "List.hpp"

template <typename T>
inline std::string toString(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

inline unsigned int myStoi(std::string str) {
    unsigned int result = 0;
    for (unsigned int i = 0; str[i] >= '0' && str[i] <= '9'; i++)
        result = result * 10 + (str[i] - '0');
    return result;
}

inline bool containsNonAlpha(std::string str) {
    for (unsigned int i = 0; i < str.length(); i++)
        if (((int)(str[i]) < 'a' || (int)(str[i]) > 'z') &&
            ((int)(str[i]) < 'A' || (int)(str[i]) > 'Z'))
            return true;
    return false;
}

// Reads a line until character '\0' and splits it
// into arguments which are pushed into the argument list.
// The splitting procedure cuts off whitespace characters and the deliminator
inline void splitLine(std::string &str, List<std::string> &args, char delim = ' ') {
    std::string result("");
    int pos = 0, len = 0;
    args.flush();
    while (str[pos] != '\0') {
        len = 0;
        // Ingore whitespace characters
        while (str[pos] == ' ' || str[pos] == '\t' || str[pos] == delim) pos++;
        while (str[pos] != ' ' && str[pos] != '\t' && str[pos] != delim && str[pos] != '\0') {
            pos++;
            len++;
        }
        result = str.substr(pos - len, len);
        args.insertLast(result);
        if (str[pos] != '\0') pos++;
    }
}

inline bool isInt(std::string str) {
    if (str.empty()) return false;
    unsigned int pos = 0;
    while (str[pos])
        if (!isdigit(str[pos++])) return false;
    return true;
}

#endif