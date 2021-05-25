#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#include <cstdlib>
#include <ctime>

#define DEFAULT_LEVELS 5
#define PROBABILITY 0.5

static bool seeded = false;

template <typename T>
class SkipList {
   private:
    int maxLevel;
    struct skipNode {
        T data;
        skipNode **nextAtLevel;
    };
    int size;
    skipNode *head, *const tail;
    const double p;

    // Creates a random number of levels up to maxLevel
    int generateLevels() {
        int level = 0;
        double probability = 0.0;
        while (probability < p && level < maxLevel) {
            probability = ((double)rand() / RAND_MAX);
            level++;
        }
        return level;
    }

    skipNode *createNode(T data, int lvls = DEFAULT_LEVELS) {
        skipNode *node = new skipNode;
        node->data = data;
        node->nextAtLevel = new skipNode *[lvls];
        for (int i = 0; i < lvls; i++)
            node->nextAtLevel[i] = tail;
        return node;
    }

    void deleteNode(skipNode *node) {
        if (!node) return;
        delete[] node->nextAtLevel;
        delete node;
    }

    // Copies the given list recursively so that
    // the final form is identical with the original
    void recCopy(skipNode *node) {
        if (node) {
            recCopy(node->nextAtLevel[0]);
            insert(node->data);
        }
    }

    // Deletes all nodes except the arbitrary head node
    void flush() {
        while (!empty()) remove(getFirst());
    }

   public:
    SkipList(int lvls = DEFAULT_LEVELS, double probability = PROBABILITY)
        : maxLevel(lvls), size(0), tail(NULL), p(PROBABILITY) {
        if (!seeded) {
            srand(time(NULL));
            seeded = true;
        }
        this->head = createNode(-1, maxLevel);
    }
    ~SkipList() {
        flush();
        delete[] head->nextAtLevel;
        delete head;
        delete tail;
    }
    SkipList(const SkipList &l);

    SkipList &operator=(const SkipList &l);

    int getMaxLevel() const { return maxLevel; }
    int getSize() const { return size; }
    T &getFirst() { return head->nextAtLevel[0]->data; }

    void insert(const T data);
    void remove(const T data);

    bool empty() const { return !size; }
    void print() const;
    T *getNode(int pos);
    T *search(const T data) const;
};

template <typename T>
SkipList<T>::SkipList(const SkipList &l)
    : maxLevel(DEFAULT_LEVELS), tail(NULL), p(PROBABILITY) {
    if (this == &l) return;
    flush();
    recCopy(l.head->nextAtLevel[0]);
}

template <typename T>
SkipList<T> &SkipList<T>::operator=(const SkipList &l) {
    if (this == &l) return *this;
    flush();
    recCopy(l.head->nextAtLevel[0]);
    return *this;
}

template <typename T>
void SkipList<T>::insert(const T data) {
    // Array of pointers on every level which will
    // be affected by the insertion of the new node
    skipNode *previousAtLevel[maxLevel];
    // Initialize them all pointing to the tail (NULL)
    for (int i = 0; i < maxLevel; i++)
        previousAtLevel[i] = tail;

    skipNode *temp = head;
    for (int i = maxLevel - 1; i >= 0; i--) {
        while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
            temp = temp->nextAtLevel[i];
        // Save the last node on the current level before we have to move lower
        previousAtLevel[i] = temp;
    }

    // Don't insert duplicates
    if (temp->data != data) {
        // Create the new node with random number of maxLevel up to max allowed
        int randomLevels = generateLevels();
        skipNode *node = createNode(data, randomLevels);

        // Traverse from the top levels to the bottom and fix pointers affected
        for (int i = randomLevels - 1; i >= 0; i--) {
            // Link the new node right after the last node we traversed in each level
            node->nextAtLevel[i] = previousAtLevel[i]->nextAtLevel[i];
            // Rearrange the pointers before new node at each level
            previousAtLevel[i]->nextAtLevel[i] = node;
        }
        this->size++;
    }
}

template <typename T>
void SkipList<T>::remove(const T data) {
    // Array of pointers on every level which will
    // be affected by the removal of the node
    skipNode *previousAtLevel[maxLevel];
    // Initialize them all pointing to the tail (NULL)
    for (int i = 0; i < maxLevel; i++)
        previousAtLevel[i] = tail;

    skipNode *temp = head;
    for (int i = maxLevel - 1; i >= 0; i--) {
        while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
            temp = temp->nextAtLevel[i];
        // Save the last node on the current level before we have to move lower
        previousAtLevel[i] = temp;
    }

    // The next node at level zero MIGHT be the one we are looking for
    // as we stopped somewhere at level zero where temp->next[0] is >= data or NULL
    // To finoud out if this is the node we're looking for, we take the next step
    temp = temp->nextAtLevel[0];

    // Delete if found
    if (temp->data == data) {
        // Traverse from the top levels to the bottom and fix pointers affected
        for (int i = maxLevel - 1; i >= 0; i--)
            // Only rearrange the nodes that we previous of the removed node at some level
            if (previousAtLevel[i]->nextAtLevel[i] == temp)
                previousAtLevel[i]->nextAtLevel[i] = temp->nextAtLevel[i];
        deleteNode(temp);
        this->size--;
    }
}

template <typename T>
void SkipList<T>::print() const {
    for (int i = maxLevel - 1; i >= 0; i--) {
        // Skip the arbitrary head node as its value is negative
        skipNode *temp = head->nextAtLevel[i];
        std::cout << "Level\t" << i << ": ";
        while (temp) {
            std::cout << temp->data << " ";
            temp = temp->nextAtLevel[i];
        }
        std::cout << std::endl;
    }
}

template <typename T>
T *SkipList<T>::getNode(int pos) {
    if (pos > size) return NULL;
    skipNode *temp = head->nextAtLevel[0];
    for (; pos > 0; pos--)
        temp = temp->nextAtLevel[0];
    T *ptr = &temp->data;
    return ptr;
}

template <typename T>
T *SkipList<T>::search(const T data) const {
    if (empty()) return NULL;
    skipNode *temp = head;
    for (int i = maxLevel - 1; i >= 0; i--)
        while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
            temp = temp->nextAtLevel[i];

    // We stopped somewhere at level 0, where the next node is either the tail
    // or its >= data, so we set temp = temp->next[0] to see if we found it.
    temp = temp->nextAtLevel[0];
    if (temp && temp->data == data) return &temp->data;
    return NULL;
}

#endif