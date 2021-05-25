#include "include/Virus.hpp"

Virus::Virus(const Virus &virus) {
    if (this == &virus) return;
    name.clear();
    name.assign(virus.getName());
}

Virus &Virus::operator=(const Virus &virus) {
    if (this == &virus) return *this;
    name.clear();
    name.assign(virus.getName());
    filter = virus.filter;
    vaccinatedList = virus.vaccinatedList;
    nonVaccinatedList = virus.nonVaccinatedList;
    return *this;
}

bool operator==(const Virus &v1, const Virus &v2) {
    return (v1.getName() == v2.getName());
}

bool operator!=(const Virus &v1, const Virus &v2) {
    return !(v1 == v2);
}

bool operator<(const Virus &v1, const Virus &v2) {
    return v1.getName() < v2.getName();
}

bool operator>(const Virus &v1, const Virus &v2) {
    return v1.getName() > v2.getName();
}

bool operator<=(const Virus &v1, const Virus &v2) {
    return v1.getName() <= v2.getName();
}

bool operator>=(const Virus &v1, const Virus &v2) {
    return v1.getName() >= v2.getName();
}

std::ostream &operator<<(std::ostream &os, const Virus &virus) {
    os << virus.getName();
    return os;
}