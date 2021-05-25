#ifndef VIRUSCOUNTRYTABLE_HPP
#define VIRUSCOUNTRYTABLE_HPP

#include "Country.hpp"
#include "Virus.hpp"

// Contains statistics per Virus-Country Combination
class VirusCountryEntry {
   private:
    Virus *virus;
    Country *country;
    unsigned int totalRegistered;
    unsigned int totalVaccinated;
    unsigned int total_0_20;
    unsigned int total_20_40;
    unsigned int total_40_60;
    unsigned int total_60_plus;
    unsigned int vaccinated_0_20;
    unsigned int vaccinated_20_40;
    unsigned int vaccinated_40_60;
    unsigned int vaccinated_60_plus;

   public:
    VirusCountryEntry() : virus(NULL), country(NULL), totalRegistered(0), totalVaccinated(0), total_0_20(0), total_20_40(0), total_40_60(0), total_60_plus(0), vaccinated_0_20(0), vaccinated_20_40(0), vaccinated_40_60(0), vaccinated_60_plus(0) {}
    ~VirusCountryEntry() {}
    VirusCountryEntry(const VirusCountryEntry &entry);

    VirusCountryEntry &operator=(const VirusCountryEntry &entry);

    Virus &getVirus() { return *virus; }
    Country &getCountry() { return *country; }
    unsigned int getTotalRegistered() const { return totalRegistered; }
    unsigned int getTotalVaccinated() const { return totalVaccinated; }
    unsigned int getTotal_0_20() const { return total_0_20; }
    unsigned int getTotal_20_40() const { return total_20_40; }
    unsigned int getTotal_40_60() const { return total_40_60; }
    unsigned int getTotal_60_plus() const { return total_60_plus; }
    unsigned int vac_0_20() const { return vaccinated_0_20; }
    unsigned int vac_20_40() const { return vaccinated_20_40; }
    unsigned int vac_40_60() const { return vaccinated_40_60; }
    unsigned int vac_60_plus() const { return vaccinated_60_plus; }

    void set(Virus *v, Country *c);
    void registerPerson(unsigned int age, std::string status);

    friend bool operator==(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend bool operator!=(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend bool operator<(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend bool operator>(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend bool operator<=(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend bool operator>=(const VirusCountryEntry &e1, const VirusCountryEntry &e2);
    friend std::ostream &operator<<(std::ostream &os, const VirusCountryEntry &entry);

    void print() const { std::cout << *this; };
};

#endif