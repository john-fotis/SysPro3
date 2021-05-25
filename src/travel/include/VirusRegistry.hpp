#ifndef VIRUSREGISTRY_HPP
#define VIRUSREGISTRY_HPP

#include <iostream>

#include "../../../include/BloomFilter.hpp"

class VirusRegistry {
   private:
    std::string name;
    BloomFilter filter;

   public:
    VirusRegistry() : name("") {}
    VirusRegistry(unsigned int bloomSize) : name(""), filter(bloomSize) {}
    ~VirusRegistry() {}
    VirusRegistry(const VirusRegistry &vRegistry);
    VirusRegistry &operator=(const VirusRegistry &vRegistry);

    std::string getName() const { return name; }
    unsigned int getBloomSize() const { return filter.getSize(); }

    void setName(const std::string &str) { name = str; }
    void initializeBloom(const VirusRegistry &vRegistry) { filter = vRegistry.filter; }
    void initializeBloom(char *array, unsigned int size) { filter.merge(array, size); }
    void copyBloom(BloomFilter &bloom) { bloom = filter; }
    void insertBloom(const std::string &str) { filter.insert(str); }
    void mergeBloom(char *array, unsigned int size) { filter.merge(array, size); }

    bool checkBloom(const std::string &str) { return filter.check(str); }
    // Parameter size is for securing we're merging samesize filters
    char *getBloom() const { return filter.getArray(); }

    friend bool operator==(const VirusRegistry &v1, const VirusRegistry &v2);
    friend bool operator!=(const VirusRegistry &v1, const VirusRegistry &v2);
    friend bool operator<(const VirusRegistry &v1, const VirusRegistry &v2);
    friend bool operator>(const VirusRegistry &v1, const VirusRegistry &v2);
    friend bool operator<=(const VirusRegistry &v1, const VirusRegistry &v2);
    friend bool operator>=(const VirusRegistry &v1, const VirusRegistry &v2);
    friend std::ostream &operator<<(std::ostream &os, const VirusRegistry &vRegistry);

    void bloomStatus() const { filter.arrayStatus(); }
    void print() const { std::cout << *this; };
};

#endif