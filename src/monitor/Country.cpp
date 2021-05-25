#include "include/Country.hpp"

Country::Country(const Country &country) {
    if (this == &country) return;
    name.clear();
    name.assign(country.getName());
}

Country &Country::operator=(const Country &country) {
    if (this == &country) return *this;
    name.clear();
    name.assign(country.getName());
    return *this;
}

bool operator==(const Country &c1, const Country &c2) {
    return (c1.getName() == c2.getName());
}

bool operator!=(const Country &c1, const Country &c2) {
    return !(c1 == c2);
}

bool operator<(const Country &c1, const Country &c2) {
    return c1.getName() < c2.getName();
}

bool operator>(const Country &c1, const Country &c2) {
    return c1.getName() > c2.getName();
}

bool operator<=(const Country &c1, const Country &c2) {
    return c1.getName() <= c2.getName();
}

bool operator>=(const Country &c1, const Country &c2) {
    return c1.getName() >= c2.getName();
}

std::ostream &operator<<(std::ostream &os, const Country &country) {
    os << country.getName();
    return os;
}