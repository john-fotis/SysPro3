#include "include/Record.hpp"

Record::Record(const Record &record) {
    if (this == &record) return;
    citizenID = record.ID();
    date = record.getDate();
}

Record &Record::operator=(const Record &record) {
    if (this == &record) return *this;
    citizenID = record.ID();
    date = record.getDate();
    return *this;
}

bool operator==(const Record &r1, const Record &r2) {
    return (r1.ID() == r2.ID());
}

bool operator!=(const Record &r1, const Record &r2) {
    return !(r1 == r2);
}

bool operator<(const Record &r1, const Record &r2) {
    return r1.ID() < r2.ID();
}

bool operator>(const Record &r1, const Record &r2) {
    return r1.ID() > r2.ID();
}

bool operator<=(const Record &r1, const Record &r2) {
    return r1.ID() <= r2.ID();
}

bool operator>=(const Record &r1, const Record &r2) {
    return r1.ID() >= r2.ID();
}

std::ostream &operator<<(std::ostream &os, const Record &record) {
    os << record.ID() << " " << record.getDate();
    return os;
}

void Record::set(unsigned int id, Date date) {
    this->citizenID = id;
    this->date = date;
}

void Record::set(unsigned int id, std::string dateStr) {
    this->citizenID = id;
    this->date.set(dateStr);
}