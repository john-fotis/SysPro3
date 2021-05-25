#ifndef REQUESTREGISTRY_HPP
#define REQUESTREGISTRY_HPP

#include <iostream>

#include "../../../include/List.hpp"
#include "Request.hpp"

class RequestRegistry {
   private:
    std::string country;
    List<Request> requests;
   public:
    RequestRegistry(std::string c = "") : country(c) {}
    ~RequestRegistry() {}
    RequestRegistry(const RequestRegistry &registry);
    RequestRegistry &operator=(const RequestRegistry &registry);

    std::string getCountry() const { return country; }
    List<Request> getRequests() const { return requests; }
    Request *getReqNode(unsigned int num) { return requests.getNode(num); }
    unsigned int getRequestsNum() const {return requests.getSize(); }

    void setCountry(std::string c) { country = c; }
    void addRequest(const Request &request) { requests.insertAscending(request); }

    friend bool operator==(const RequestRegistry &r1, const RequestRegistry &r2);
    friend bool operator!=(const RequestRegistry &r1, const RequestRegistry &r2);
    friend bool operator<(const RequestRegistry &r1, const RequestRegistry &r2);
    friend bool operator>(const RequestRegistry &r1, const RequestRegistry &r2);
    friend bool operator<=(const RequestRegistry &r1, const RequestRegistry &r2);
    friend bool operator>=(const RequestRegistry &r1, const RequestRegistry &r2);
    friend std::ostream &operator<<(std::ostream &os, const RequestRegistry &rRegistry);

    void print() const { std::cout << *this; };
};

#endif