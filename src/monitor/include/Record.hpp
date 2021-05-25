#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>

#include "../../../include/Date.hpp"

class Record {
   private:
    unsigned int citizenID;
    Date date;

   public:
    Record(int id = 0) : citizenID(id) {}
    ~Record() {}
    Record(const Record &record);
    Record &operator=(const Record &record);

    unsigned int ID() const { return citizenID; }
    std::string getDate() const {
        std::string str("");
        date.get(str);
        return str;
    }

    void setID(unsigned int id) { citizenID = id; }
    void setDate(Date &d) { date = d; }

    void set(unsigned int id, Date date);
    void set(unsigned int id, std::string dateStr);

    friend bool operator==(const Record &r1, const Record &r2);
    friend bool operator!=(const Record &r1, const Record &r2);
    friend bool operator<(const Record &r1, const Record &r2);
    friend bool operator>(const Record &r1, const Record &r2);
    friend bool operator<=(const Record &r1, const Record &r2);
    friend bool operator>=(const Record &r1, const Record &r2);
    friend std::ostream &operator<<(std::ostream &os, const Record &record);

    void print() const { std::cout << *this; };
};

#endif