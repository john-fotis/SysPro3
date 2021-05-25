#include "include/VirusRegistry.hpp"

VirusRegistry::VirusRegistry(const VirusRegistry &vRegistry) {
    if (this == &vRegistry) return;
    name.clear();
    name.assign(vRegistry.getName());
}

VirusRegistry &VirusRegistry::operator=(const VirusRegistry &vRegistry) {
    if (this == &vRegistry) return *this;
    name.clear();
    name.assign(vRegistry.getName());
    filter = vRegistry.filter;
    return *this;
}

bool operator==(const VirusRegistry &v1, const VirusRegistry &v2) {
    return (v1.getName() == v2.getName());
}

bool operator!=(const VirusRegistry &v1, const VirusRegistry &v2) {
    return !(v1 == v2);
}

bool operator<(const VirusRegistry &v1, const VirusRegistry &v2) {
    return v1.getName() < v2.getName();
}

bool operator>(const VirusRegistry &v1, const VirusRegistry &v2) {
    return v1.getName() > v2.getName();
}

bool operator<=(const VirusRegistry &v1, const VirusRegistry &v2) {
    return v1.getName() <= v2.getName();
}

bool operator>=(const VirusRegistry &v1, const VirusRegistry &v2) {
    return v1.getName() >= v2.getName();
}

std::ostream &operator<<(std::ostream &os, const VirusRegistry &vRegistry) {
    os << vRegistry.getName();
    return os;
}