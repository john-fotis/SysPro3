#ifndef Person_HPP
#define Person_HPP

#include <iostream>

#include "../../../include/Date.hpp"
#include "Country.hpp"

class Person {
   private:
    unsigned int id;
    std::string firstName;
    std::string lastName;
    Country *country;
    unsigned int age;

   public:
    Person() : id(0), firstName(""), lastName(""), country(NULL), age(0) {}
    ~Person() {}
    Person(const Person &person);
    Person &operator=(const Person &person);

    unsigned int ID() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    Country &getCountry() const { return *country; }
    unsigned int getAge() const { return age; }

    void setID(unsigned int i) { id = i; }
    void setName(std::string name) { firstName = name; }
    void setSurname(std::string surname) { lastName = surname; }
    void setCountry(Country *c) { country = c; }
    void set(unsigned int id, std::string name, std::string surname,
             Country *country, unsigned int age);

    friend bool operator==(const Person &p1, const Person &p2);
    friend bool operator!=(const Person &p1, const Person &p2);
    friend bool operator<(const Person &p1, const Person &p2);
    friend bool operator>(const Person &p1, const Person &p2);
    friend bool operator<=(const Person &p1, const Person &p2);
    friend bool operator>=(const Person &p1, const Person &p2);
    friend std::ostream &operator<<(std::ostream &os, const Person &person);

    bool isIdentical(const Person &person);
    void print() const { std::cout << *this; };
};

#endif