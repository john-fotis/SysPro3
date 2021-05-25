#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

#include "../../../include/Date.hpp"
#include "VirusRegistry.hpp"

class Request {
   private:
    VirusRegistry *virus;
    bool status;
    Date requestDate;

   public:
    Request(bool s = false) : virus(NULL), status(s) {}
    ~Request() {}
    Request(const Request &request);
    Request &operator=(const Request &request);

    void setVirus(VirusRegistry *v) { virus = v; }
    void setStatus(bool s) { status = s; }
    void setDate(Date date) { requestDate = date; }
    void set(VirusRegistry *virus, bool s, Date date);

    VirusRegistry &getVirus() const { return *virus; }
    bool getStatus() const { return status; }
    Date getDate() const { return requestDate; }

    friend bool operator==(const Request &r1, const Request &r2);
    friend bool operator!=(const Request &r1, const Request &r2);
    friend bool operator<(const Request &r1, const Request &r2);
    friend bool operator>(const Request &r1, const Request &r2);
    friend bool operator<=(const Request &r1, const Request &r2);
    friend bool operator>=(const Request &r1, const Request &r2);
    friend std::ostream &operator<<(std::ostream &os, const Request &rRegistry);

    void print() const { std::cout << *this; };
};

#endif