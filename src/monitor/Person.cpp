#include "include/Person.hpp"

Person::Person(const Person &person) {
    if (this == &person) return;
    id = person.ID();
    firstName = person.getFirstName();
    lastName = person.getLastName();
    country = &person.getCountry();
    age = person.getAge();
}

Person &Person::operator=(const Person &person) {
    if (this == &person) return *this;
    id = person.ID();
    firstName = person.getFirstName();
    lastName = person.getLastName();
    country = &person.getCountry();
    age = person.getAge();
    return *this;
}

void Person::set(unsigned int id, std::string name, std::string surname,
                 Country *country, unsigned int age) {
    this->id = id;
    firstName = name;
    lastName = surname;
    this->country = country;
    this->age = age;
}

bool operator==(const Person &p1, const Person &p2) {
    return (p1.ID() == p2.ID());
}

bool operator!=(const Person &p1, const Person &p2) {
    return !(p1 == p2);
}

bool operator<(const Person &p1, const Person &p2) {
    return p1.ID() < p2.ID();
}

bool operator>(const Person &p1, const Person &p2) {
    return p1.ID() > p2.ID();
}

bool operator<=(const Person &p1, const Person &p2) {
    return p1.ID() <= p2.ID();
}

bool operator>=(const Person &p1, const Person &p2) {
    return p1.ID() >= p2.ID();
}

std::ostream &operator<<(std::ostream &os, const Person &person) {
    os << person.ID() << " "
       << person.getFirstName() << " "
       << person.getLastName() << " "
       << person.getCountry() << " "
       << person.getAge();
    return os;
}

bool Person::isIdentical(const Person &person) {
    return (
        this->ID() == person.ID() &&
        this->getFirstName() == person.getFirstName() &&
        this->getLastName() == person.getLastName() &&
        this->getCountry() == person.getCountry() &&
        this->getAge() == person.getAge());
}