#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "List.hpp"
#include "hashFunctions.hpp"

#define TABLE_CAPACITY 1000

template <typename T>
class HashTable {
   private:
    unsigned int size;
    unsigned int totalEntries;
    List<T> *table;  // Array of collision buckets used for the chains

    unsigned long hashFunction(std::string key) {
        return hash_i((unsigned char *)key.c_str(), key.length()) % size;
    }

   public:
    HashTable(unsigned int capacity = TABLE_CAPACITY)
        : size(capacity), totalEntries(0) { table = new List<T>[size]; }
    ~HashTable() { delete[] table; }
    HashTable (const HashTable &t);
    HashTable &operator=(const HashTable &t);

    unsigned int getSize() const { return size; }
    unsigned int getTotalEntries() const { return totalEntries; }

    void insert(std::string key, T node);
    void remove(std::string key, T node);

    T *search(std::string key, T node);
    List<T> getBucket(std::string key);
    void print() const;
};

template <typename T>
HashTable<T>::HashTable(const HashTable<T> &t) {
    if (this == &t) return;
    for (unsigned int bucket = 0; bucket < size; bucket++)
        table[bucket].flush();
    delete[] table;
    size = t.getSize();
    totalEntries = t.getTotalEntries();
    table = new List<T>[size];
    for (unsigned int bucket = 0; bucket < size; bucket++)
        table[bucket] = t.table[bucket];
    return;
}

template <typename T>
HashTable<T> &HashTable<T>::operator=(const HashTable<T> &t) {
    if (this == &t) return *this;
    for (unsigned int bucket = 0; bucket < size; bucket++)
        table[bucket].flush();
    delete[] table;
    size = t.getSize();
    totalEntries = t.getTotalEntries();
    table = new List<T>[size];
    for (unsigned int bucket = 0; bucket < size; bucket++)
        table[bucket] = t.table[bucket];
    return *this;
}

template <typename T>
void HashTable<T>::insert(std::string key, T node) {
    unsigned long index = hashFunction(key);
    table[index].insertFirst(node);
    totalEntries++;
}

template <typename T>
void HashTable<T>::remove(std::string key, T node) {
    unsigned long index = hashFunction(key);
    table[index].popValue(node);
    totalEntries--;
}

template <typename T>
T *HashTable<T>::search(std::string key, T node) {
    unsigned long index = hashFunction(key);
    T *temp = NULL;
    temp = table[index].search(node);
    return temp;
}

template <typename T>
List<T> HashTable<T>::getBucket(std::string key) {
    unsigned long index = hashFunction(key);
    return table[index];
}

template <typename T>
void HashTable<T>::print() const {
    for (unsigned int bucket = 0; bucket < size; bucket++)
        if (!table[bucket].empty()) {
            std::cout << "|| Index " << bucket << " || ";
            table[bucket].print();
        }
}

#endif