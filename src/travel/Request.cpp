#include "include/Request.hpp"

Request::Request(const Request &request) {
    if (this == &request) return;
    virus = &request.getVirus();
    status = request.getStatus();
    requestDate = request.getDate();
}

Request &Request::operator=(const Request &request) {
    if (this == &request) return *this;
    virus = &request.getVirus();
    status = request.getStatus();
    requestDate = request.getDate();
    return *this;
}

void Request::set(VirusRegistry *v, bool s, Date date) {
    virus = v;
    status = s;
    requestDate = date;
}

bool operator==(const Request &r1, const Request &r2) {
    return (r1.getVirus() == r2.getVirus());
}

bool operator!=(const Request &r1, const Request &r2) {
    return !(r1 == r2);
}

bool operator<(const Request &r1, const Request &r2) {
    return r1.getVirus() < r2.getVirus();
}

bool operator>(const Request &r1, const Request &r2) {
    return r1.getVirus() > r2.getVirus();
}

bool operator<=(const Request &r1, const Request &r2) {
    return r1.getVirus() <= r2.getVirus();
}

bool operator>=(const Request &r1, const Request &r2) {
    return r1.getVirus() >= r2.getVirus();
}

std::ostream &operator<<(std::ostream &os, const Request &request) {
    os << request.getStatus() << " " << request.getVirus() << " " << request.getDate();
    return os;
}