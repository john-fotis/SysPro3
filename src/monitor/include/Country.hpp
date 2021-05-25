#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include <iostream>

class Country {
   private:
    std::string name;

   public:
    Country() : name("") {}
    ~Country() {}
    Country(const Country &country);
    Country &operator=(const Country &country);

    std::string getName() const { return name; }

    void setName(const std::string &str) {
        name.clear();
        name.assign(str);
    }

    friend bool operator==(const Country &c1, const Country &c2);
    friend bool operator!=(const Country &c1, const Country &c2);
    friend bool operator<(const Country &c1, const Country &c2);
    friend bool operator>(const Country &c1, const Country &c2);
    friend bool operator<=(const Country &c1, const Country &c2);
    friend bool operator>=(const Country &c1, const Country &c2);
    friend std::ostream &operator<<(std::ostream &os, const Country &country);

    void print() const { std::cout << *this; };
};

#endif