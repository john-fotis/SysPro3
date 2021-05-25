#ifndef VIRUS_HPP
#define VIRUS_HPP

#include <iostream>

#include "../../../include/BloomFilter.hpp"
#include "../../../include/SkipList.hpp"
#include "Record.hpp"

class Virus {
   private:
    std::string name;
    BloomFilter filter;
    SkipList<Record> vaccinatedList;
    SkipList<int> nonVaccinatedList;

   public:
    Virus() : name("") {}
    Virus(unsigned int bloomSize) : name(""), filter(bloomSize) {}
    ~Virus() {}
    Virus(const Virus &virus);
    Virus &operator=(const Virus &virus);

    std::string getName() const { return name; }
    unsigned int getBloomSize() const { return filter.getSize(); }
    unsigned int getVaccinatedListSize() const { return vaccinatedList.getSize(); }
    unsigned int getNonVaccinatedListSize() const { return nonVaccinatedList.getSize(); }

    void setName(const std::string &str) { name = str; }
    void initializeBloom(const Virus &virus) { filter = virus.filter; }
    void copyBloom(BloomFilter &bloom) { bloom = filter; }

    void insertBloom(const std::string &str) { filter.insert(str); }
    bool checkBloom(const std::string &str) { return filter.check(str); }
    void bloomStatus() const { filter.arrayStatus(); }
    char *getBloom() const { return filter.getArray(); }

    void insertVaccinatedList(const Record &record) { vaccinatedList.insert(record); }
    void insertNonVaccinatedList(const int id) { nonVaccinatedList.insert(id); }
    void removeVaccinatedList(const Record &record) { vaccinatedList.remove(record); }
    void removeNonVaccinatedList(const int id) { nonVaccinatedList.remove(id); }

    Record *searchVaccinatedList(const Record record) { return vaccinatedList.search(record); }
    int *searchNonVaccinatedList(const int id) { return nonVaccinatedList.search(id); }
    // Used to read each record of the skiplist at given position
    Record *getPositiveRecordNumber(unsigned int num) { return vaccinatedList.getNode(num); }
    int *getNegativeRecordNumber(unsigned int num) { return nonVaccinatedList.getNode(num); }

    friend bool operator==(const Virus &v1, const Virus &v2);
    friend bool operator!=(const Virus &v1, const Virus &v2);
    friend bool operator<(const Virus &v1, const Virus &v2);
    friend bool operator>(const Virus &v1, const Virus &v2);
    friend bool operator<=(const Virus &v1, const Virus &v2);
    friend bool operator>=(const Virus &v1, const Virus &v2);
    friend std::ostream &operator<<(std::ostream &os, const Virus &virus);

    void print() const { std::cout << *this; };
};

#endif