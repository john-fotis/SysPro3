#include "include/MonitorInfo.hpp"

MonitorInfo::MonitorInfo(const MonitorInfo &monitor) {
    if (this == &monitor) return;
    pid = monitor.PID();
    sock = monitor.getSocket();
    server = monitor.server;
    countries = monitor.countries;
}

MonitorInfo &MonitorInfo::operator=(const MonitorInfo &monitor) {
    if (this == &monitor) return *this;
    pid = monitor.PID();
    sock = monitor.getSocket();
    server = monitor.server;
    countries = monitor.countries;
    return *this;
}

bool operator==(const MonitorInfo &m1, const MonitorInfo &m2) {
    return (m1.PID() == m2.PID());
}

bool operator!=(const MonitorInfo &m1, const MonitorInfo &m2) {
    return !(m1 == m2);
}

bool operator<(const MonitorInfo &m1, const MonitorInfo &m2) {
    return m1.PID() < m2.PID();
}

bool operator>(const MonitorInfo &m1, const MonitorInfo &m2) {
    return m1.PID() > m2.PID();
}

bool operator<=(const MonitorInfo &m1, const MonitorInfo &m2) {
    return m1.PID() <= m2.PID();
}

bool operator>=(const MonitorInfo &m1, const MonitorInfo &m2) {
    return m1.PID() >= m2.PID();
}

std::ostream &operator<<(std::ostream &os, const MonitorInfo &monitor) {
    os  << monitor.PID() << " " << monitor.getSocket() << " "
        << monitor.getFamily() << " " << monitor.getPort() << std::endl;
    monitor.countries.print();
    return os;
}