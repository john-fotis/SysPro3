#include "include/RequestRegistry.hpp"

RequestRegistry::RequestRegistry(const RequestRegistry &rRegistry) {
    if (this == &rRegistry) return;
    country = rRegistry.getCountry();
    requests = rRegistry.requests;
    return;
}

RequestRegistry &RequestRegistry::operator=(const RequestRegistry &rRegistry) {
    if (this == &rRegistry) return *this;
    country = rRegistry.getCountry();
    requests = rRegistry.requests;
    return *this;
}

bool operator==(const RequestRegistry &r1, const RequestRegistry &r2) {
    return (r1.getCountry() == r2.getCountry());
}

bool operator!=(const RequestRegistry &r1, const RequestRegistry &r2) {
    return !(r1 == r2);
}

bool operator<(const RequestRegistry &r1, const RequestRegistry &r2) {
    return r1.getCountry() < r2.getCountry();
}

bool operator>(const RequestRegistry &r1, const RequestRegistry &r2) {
    return r1.getCountry() > r2.getCountry();
}

bool operator<=(const RequestRegistry &r1, const RequestRegistry &r2) {
    return r1.getCountry() <= r2.getCountry();
}

bool operator>=(const RequestRegistry &r1, const RequestRegistry &r2) {
    return r1.getCountry() >= r2.getCountry();
}

std::ostream &operator<<(std::ostream &os, const RequestRegistry &rRegistry) {
    os << rRegistry.getCountry() << " ";
    rRegistry.requests.print();
    return os;
}