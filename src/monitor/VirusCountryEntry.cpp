#include "include/VirusCountryEntry.hpp"

VirusCountryEntry::VirusCountryEntry(const VirusCountryEntry &entry) {
    if (this == &entry) return;
    virus = entry.virus;
    country = entry.country;
    totalRegistered = entry.getTotalRegistered();
    total_0_20 = entry.getTotal_0_20();
    total_20_40 = entry.getTotal_20_40();
    total_40_60 = entry.getTotal_40_60();
    total_60_plus = entry.getTotal_60_plus();
    vaccinated_0_20 = entry.vac_0_20();
    vaccinated_20_40 = entry.vac_20_40();
    vaccinated_40_60 = entry.vac_40_60();
    vaccinated_60_plus = entry.vac_60_plus();
}

VirusCountryEntry &VirusCountryEntry::operator=(const VirusCountryEntry &entry) {
    if (this == &entry) return *this;
    virus = entry.virus;
    country = entry.country;
    totalRegistered = entry.getTotalRegistered();
    total_0_20 = entry.getTotal_0_20();
    total_20_40 = entry.getTotal_20_40();
    total_40_60 = entry.getTotal_40_60();
    total_60_plus = entry.getTotal_60_plus();
    this->vaccinated_0_20 = entry.vac_0_20();
    this->vaccinated_20_40 = entry.vac_20_40();
    this->vaccinated_40_60 = entry.vac_40_60();
    this->vaccinated_60_plus = entry.vac_60_plus();
    return *this;
}

void VirusCountryEntry::set(Virus *v, Country *c) {
    virus = v;
    country = c;
    totalRegistered = 0;
    total_0_20 = 0;
    total_20_40 = 0;
    total_40_60 = 0;
    total_60_plus = 0;
    vaccinated_0_20 = 0;
    vaccinated_20_40 = 0;
    vaccinated_40_60 = 0;
    vaccinated_60_plus = 0;
}

void VirusCountryEntry::registerPerson(unsigned int age, std::string status) {
    if (age <= 0) return;

    totalRegistered++;
    bool vaccinated = !status.compare("YES");
    if (vaccinated) totalVaccinated++;

    unsigned int category = 0;
    if (age <= 20)
        category = 1;
    else if (age <= 40)
        category = 2;
    else if (age <= 60)
        category = 3;
    else
        category = 4;

    switch (category) {
        case 1:
            total_0_20++;
            if (vaccinated) vaccinated_0_20++;
            break;
        case 2:
            total_20_40++;
            if (vaccinated) vaccinated_20_40++;
            break;
        case 3:
            total_40_60++;
            if (vaccinated) vaccinated_40_60++;
            break;
        case 4:
            total_60_plus++;
            if (vaccinated) vaccinated_60_plus++;
            break;
        default:
            break;
    }
}

bool operator==(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return (*e1.virus == *e2.virus && *e1.country == *e2.country);
}

bool operator!=(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return !(e1 == e2);
}

bool operator<(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return (*e1.virus < *e1.virus);
}

bool operator>(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return (*e1.virus > *e1.virus);
}

bool operator<=(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return (*e1.virus <= *e1.virus);
}

bool operator>=(const VirusCountryEntry &e1, const VirusCountryEntry &e2) {
    return (*e1.virus >= *e1.virus);
}

std::ostream &operator<<(std::ostream &os, const VirusCountryEntry &entry) {
    os << *entry.virus << " " << *entry.country;
    return os;
}