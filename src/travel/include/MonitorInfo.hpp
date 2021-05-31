#ifndef MONITORINFO_HPP
#define MONITORINFO_HPP

#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>

#include "../../../include/AppStandards.hpp"
#include "../../../include/List.hpp"

class MonitorInfo {
private:
    pid_t pid;
    int sock;
    sockaddr_in server;
    List<std::string> countries;

public:
    MonitorInfo(pid_t p = 0, sa_family_t family = AF_INET, uint32_t addr = htonl(INADDR_ANY))
    : pid(p) { server.sin_family = family; server.sin_addr.s_addr = htonl(addr); }
    ~MonitorInfo() { countries.flush(); }
    MonitorInfo(const MonitorInfo &monitor);
    MonitorInfo &operator=(const MonitorInfo &monitor);

    int PID () const { return pid; }
    int getSocket() const { return sock; }
    socklen_t getSockLen() { return sizeof(server); }
    uint16_t getPort() const { return ntohs(server.sin_port); }
    sockaddr_in &getServer() { return server; }
    unsigned int getAddressLen() const { return sizeof(server.sin_addr.s_addr); }
    sa_family_t getFamily() const { return server.sin_family; }
    List<std::string> &getCountries() { return countries; }
    std::string getNthCountry(unsigned int pos) {
        if (pos >= countries.getSize()) return NULL;
        return *countries.getNode(pos);
    }
    unsigned int countriesNumber() { return countries.getSize(); }

    void setPID(unsigned int p) { pid = p; }
    void setSocket(int s) { sock = s; }
    void setFamily(sa_family_t family = AF_INET) { server.sin_family = family; }
    void setAddress(uint32_t addr = ntohl(INADDR_ANY)) { server.sin_addr.s_addr = addr; }
    void setAddress(hostent *remHost) { memcpy(&server.sin_addr, remHost->h_addr, remHost->h_length); }
    void setPort(uint16_t p) { server.sin_port = p; }
    void setCountries(const List<std::string> &c) { countries = c; }
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