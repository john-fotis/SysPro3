#ifndef MONITORINFO_HPP
#define MONITORINFO_HPP

#include <iostream>
#include "../../../include/AppStandards.hpp"
#include "../../../include/List.hpp"

class MonitorInfo {
private:
    pid_t pid;
    int sock;
    unsigned long port;
    List<std::string> countries;

public:
    MonitorInfo(pid_t p = 0) : pid(p) {}
    ~MonitorInfo() {}
    MonitorInfo(const MonitorInfo &monitor);
    MonitorInfo &operator=(const MonitorInfo &monitor);

    int PID () const { return pid; }
    int getSocket() const { return sock; }
    unsigned long getPort() const { return port; }
    List<std::string> &getCountries() { return countries; }
    unsigned int countriesNumber() { return countries.getSize(); }

    void setPID(unsigned int p) { pid = p; }
    void setSocket(int s) { sock = s; }
    void setPort(unsigned long p) { port = p; }
    void insertCountry(std::string country) { countries.insertAscending(country); }

    friend bool operator==(const MonitorInfo &m1, const MonitorInfo &m2);
    friend bool operator!=(const MonitorInfo &m1, const MonitorInfo &m2);
    friend bool operator<(const MonitorInfo &m1, const MonitorInfo &m2);
    friend bool operator>(const MonitorInfo &m1, const MonitorInfo &m2);
    friend bool operator<=(const MonitorInfo &m1, const MonitorInfo &m2);
    friend bool operator>=(const MonitorInfo &m1, const MonitorInfo &m2);
    friend std::ostream &operator<<(std::ostream &os, const MonitorInfo &monitor);

    std::string *searchCountry(std::string name) { return countries.search(name); }
    void print() const { std::cout << *this; };
};

#endif