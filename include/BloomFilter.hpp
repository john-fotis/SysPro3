#ifndef BLOOMFILTER_HPP
#define BLOOMFILTER_HPP

#define CHAR_SIZE (sizeof(char) * 8)  // bits
#define BITS_IN_BYTE 8
#define HASH_FUNCTIONS_NUMBER 16

#include <cstring>
#include <iostream>

#include "hashFunctions.hpp"

class BloomFilter {
   private:
    unsigned int size;  // in bits!
    unsigned int hashFunctionsNumber;
    char *bitArray;

    // Apply logical OR to the array with a mask (Mask Type: 0-0-0-0-0-0-0-0-1)
    void set(unsigned int pos) { bitArray[pos / CHAR_SIZE] |= (1 << pos % CHAR_SIZE); }
    // Apply logical AND to the array with a mask (Mask Type: 1-1-1-1-1-1-1-1-0)
    void reset(unsigned int pos) { bitArray[pos / CHAR_SIZE] &= ~(1 << pos % CHAR_SIZE); }
    // Check the result logical AND to the array with a mask (Mask Type: 0-0-0-0-0-0-0-0-1)
    bool checkBit(unsigned int pos) const { return ((bitArray[pos / CHAR_SIZE]) & (1 << pos % CHAR_SIZE)); }

   public:
    // Input size of bitarray is expected in bytes, NOT bits
    BloomFilter(unsigned int sz = 1000, int functions = 16)
        : size(sz * BITS_IN_BYTE), hashFunctionsNumber(functions) {
        bitArray = new char[sz];  // Allocating in bytes!
        for (unsigned int pos = 0; pos < size; pos++) reset(pos);
    }
    ~BloomFilter() { delete[] bitArray; }

    BloomFilter(const BloomFilter &filter) {
        if (this == &filter) return;
        size = filter.getSize();
        delete[] bitArray;
        bitArray = new char[size / BITS_IN_BYTE];  // the size we copied was in bits!
        hashFunctionsNumber = filter.getFunctionsNumber();
        for (unsigned int pos = 0; pos < size; pos++)
            if (filter.checkBit(pos))
                set(pos);
            else
                reset(pos);
    }

    BloomFilter &operator=(const BloomFilter &filter) {
        if (this == &filter) return *this;
        size = filter.getSize();
        delete[] bitArray;
        bitArray = new char[size / BITS_IN_BYTE];  // the size we copied was in bits!
        hashFunctionsNumber = filter.getFunctionsNumber();
        for (unsigned int pos = 0; pos < size; pos++)
            if (filter.checkBit(pos))
                set(pos);
            else
                reset(pos);
        return *this;
    }

    unsigned int getSize() const { return size; }
    unsigned int getFunctionsNumber() const { return hashFunctionsNumber; }
    char *getArray() const { return bitArray; }

    void insert(const std::string &input) {
        // Insert input if we know it's NOT already present
        if (!check(input)) {
            unsigned long hash = 0;
            for (unsigned int pos = 0; pos < hashFunctionsNumber; pos++) {
                hash = hash_i((unsigned char *)input.c_str(), pos);
                set(hash % size);
            }
        }
    }

    bool check(const std::string &input) {
        bool found = true;
        unsigned long hash = 0;
        for (unsigned int pos = 0; pos < hashFunctionsNumber; pos++) {
            hash = hash_i((unsigned char *)input.c_str(), pos);
            found = found && checkBit(hash % size);
        }
        return found;
    }

    // Overwrites the bitArray with given array of the same size
    void init(const char *array, unsigned int sz) {
        if ((sz * BITS_IN_BYTE) != size) return;
        memcpy(bitArray, array, size / BITS_IN_BYTE);
    }

    // Merges the bitArray with given array of the same size
    void merge(const char *array, unsigned int sz) {
        if ((sz * BITS_IN_BYTE) != size) return;
        for (unsigned int pos = 0; pos < size; pos++)
            bitArray[pos/CHAR_SIZE] |= array[pos/CHAR_SIZE];
    }

    // Prints the bitArray
    void arrayStatus() const {
        for (unsigned int pos = 0; pos < size; pos++)
            std::cout << (checkBit(pos) == true) << " ";
        std::cout << std::endl;
    }
};

#endif