#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "List.hpp"

template <typename T>
class Queue {
   private:
    unsigned int maxSize;
    List<T> qNodes;
   public:
    Queue(unsigned int maxSz) : maxSize(maxSz) {}
    ~Queue() { T data; while(!empty()) dequeue(data); }

    bool empty() const { return qNodes.empty(); }
    bool full() const { return qNodes.getSize() == maxSize; }

    bool enqueue(const T &data) {
        if (full()) return false;
        qNodes.insertLast(data);
        return true;
    }
    bool dequeue(T &data) {
        if (empty()) return false;
        data = qNodes.getFirst();
        qNodes.popFirst();
        return true;
    }
    void print() const { qNodes.print(); }
};

#endif