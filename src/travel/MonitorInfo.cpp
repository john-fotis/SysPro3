#include "include/MonitorInfo.hpp"

MonitorInfo::MonitorInfo(const MonitorInfo &monitor) {
    if (this == &monitor) return;
    pid = monitor.PID();
    countries = monitor.countries;
}

MonitorInfo &MonitorInfo::operator=(const MonitorInfo &monitor) {
    if (this == &monitor) return *this;
    pid = monitor.PID();
    countries = monitor.countries;
    return *this;
}

bool operator==(const MonitorInfo &r1, const MonitorInfo &r2) {
    return (r1.PID() == r2.PID());
}

bool operator!=(const MonitorInfo &r1, const MonitorInfo &r2) {
    return !(r1 == r2);
}

bool operator<(const MonitorInfo &r1, const MonitorInfo &r2) {
    return r1.PID() < r2.PID();
}

bool operator>(const MonitorInfo &r1, const MonitorInfo &r2) {
    return r1.PID() > r2.PID();
}

bool operator<=(const MonitorInfo &r1, const MonitorInfo &r2) {
    return r1.PID() <= r2.PID();
}

bool operator>=(const MonitorInfo &r1, const MonitorInfo &r2) {
    return r1.PID() >= r2.PID();
}

std::ostream &operator<<(std::ostream &os, const MonitorInfo &monitor) {
    os  << monitor.PID() << std::endl;
    monitor.countries.print();
    return os;
}