#ifndef LIST_HPP
#define LIST_HPP

#include <iostream>

template <typename T>
class List {
   private:
    struct listNode {
        T data;
        listNode *next;
    };
    unsigned int size;
    listNode *head;
    listNode *tail;

    listNode *createNode(T data) {
        listNode *node = new listNode();
        node->data = data;
        return node;
    }

    // Copies the given list recursively so that
    // the final form is identical with the original
    void recCopy(listNode *node) {
        if (node) {
            recCopy(node->next);
            insertFirst(node->data);
        }
    }

   public:
    List() : size(0), head(NULL), tail(NULL) {}
    List(const List &l);
    ~List() { flush(); }
    List &operator=(const List &l);
    unsigned int getSize() const { return size; }
    T &getFirst() { return head->data; }
    T &getLast() { return tail->data; }

    void insertFirst(const T data);
    void insertLast(const T data);
    void insertAscending(const T data);
    void insertDescending(const T data);
    void popFirst();
    void popLast();
    void popValue(T data);

    List<T> &sortAscending();
    List<T> &sortDescending();
    List<T> &invert();

    bool empty() const { return (size == 0 && head == NULL && tail == NULL); }
    void flush() {
        while (!empty()) popFirst();
    }  // Empties the list

    void print() const;
    T *getNode(unsigned int pos);
    T *search(const T data) const;
    bool contains(const T &data) const;
};

template <typename T>
List<T>::List(const List<T> &l) : size(0), head(NULL), tail(NULL) {
    if (this==&l) return;
    flush();
    recCopy(l.head);
}

template <typename T>
List<T> &List<T>::operator=(const List<T> &l) {
    if (this == &l) return *this;
    flush();
    recCopy(l.head);
    return *this;
}

template <typename T>
void List<T>::insertFirst(T data) {
    listNode *node = createNode(data);
    if (!this->head) {
        node->next = NULL;
        this->tail = node;
    } else {
        node->next = this->head;
    }
    this->head = node;
    this->size++;
}

template <typename T>
void List<T>::insertLast(T data) {
    listNode *node = createNode(data);
    if (!this->head) {
        this->head = node;
    } else {
        this->tail->next = node;
    }
    this->tail = node;
    this->size++;
}

template <typename T>
void List<T>::insertAscending(T data) {
    if (empty() || this->head->data >= data) {
        insertFirst(data);
    } else if (this->tail->data <= data) {
        insertLast(data);
    } else if (this->size == 1) {
        (this->head->data >= data) ? insertFirst(data) : insertLast(data);
    } else {
        listNode *node = createNode(data);
        listNode *current = this->head;
        listNode *next = current->next;
        for (; next->data < data; next = current->next)
            current = current->next;
        node->next = next;
        current->next = node;
        this->size++;
    }
}

template <typename T>
void List<T>::insertDescending(T data) {
    if (empty() || this->head->data < data) {
        insertFirst(data);
    } else if (this->tail->data > data) {
        insertLast(data);
    } else if (this->size == 1) {
        (this->head->data < data) ? insertFirst(data) : insertLast(data);
    } else {
        listNode *node = createNode(data);
        listNode *current = this->head;
        listNode *next = current->next;
        for (; next->data >= data; next = current->next)
            current = current->next;
        node->next = next;
        current->next = node;
        this->size++;
    }
}

template <typename T>
void List<T>::popFirst() {
    if (empty()) return;
    listNode *temp = this->head;
    this->head = this->head->next;
    if (!this->head) {
        this->tail = NULL;
    } else {
        if (!this->head->next)
            this->tail = this->head;
    }
    delete temp;
    this->size--;
}

template <typename T>
void List<T>::popLast() {
    if (empty()) return;
    listNode *temp = this->tail;
    if (this->head->next) {
        listNode *newTail = this->head;
        while (newTail->next != this->tail)
            newTail = newTail->next;
        newTail->next = NULL;
        this->tail = newTail;
    } else {
        this->head = this->tail = NULL;
    }
    delete temp;
    this->size--;
}

template <typename T>
void List<T>::popValue(T data) {
    if (empty() || !search(data)) return;
    if (this->head->data == data) {
        popFirst();
    } else if (this->tail->data == data) {
        popLast();
    } else {
        listNode *current = this->head, *previous = NULL;
        while (current && current->data != data) {
            previous = current;
            current = current->next;
        }
        previous->next = current->next;
        delete current;
        size--;
    }
}

template <typename T>
List<T> &List<T>::sortAscending() {
    if (empty()) return *this;
    List<T> newList;
    listNode *temp = this->head;
    for (; temp; temp = temp->next)
        newList.insertAscending(temp->data);
    this->flush();
    this->recCopy(newList.head);
    return *this;
}

template <typename T>
List<T> &List<T>::sortDescending() {
    this->sortAscending();
    this->invert();
    return *this;
}

template <typename T>
List<T> &List<T>::invert() {
    if (empty()) return *this;
    listNode *current = this->head;
    listNode *next = NULL, *previous = NULL;
    while (current) {
        next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }
    this->head = previous;
    return *this;
}

template <typename T>
void List<T>::print() const {
    if (empty()) return;
    listNode *temp = this->head;
    for (; temp->next; temp = temp->next)
        std::cout << temp->data << " ==> ";
    std::cout << temp->data << std::endl;
}

template <typename T>
T *List<T>::getNode(unsigned int pos) {
    if (pos > size) return NULL;
    listNode *temp = head;
    for (; pos > 0; pos--)
        temp = temp->next;
    T *ptr = &temp->data;
    return ptr;
}

template <typename T>
T *List<T>::search(const T data) const {
    if (empty()) return NULL;
    listNode *temp = this->head;
    T *ptr = NULL;
    while (temp && temp->data != data)
        temp = temp->next;
    if (temp) ptr = &temp->data;
    return ptr;
}

template <typename T>
bool List<T>::contains(const T &data) const {
    if (empty()) return false;
    listNode *temp = this->head;
    while (temp && temp->data != data)
        temp = temp->next;
    return temp;
}

#endif