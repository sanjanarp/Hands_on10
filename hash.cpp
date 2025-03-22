#include <iostream>
#include <cmath>

using namespace std;

const float LOAD_FACTOR_GROW = 1.0;
const float LOAD_FACTOR_SHRINK = 0.25;

typedef int (*HashFunc)(int key, int capacity);

// Node for doubly linked list
struct Node {
    int key, value;
    Node* prev;
    Node* next;
    Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};

// Doubly linked list
struct LinkedList {
    Node* head;
    LinkedList() : head(nullptr) {}

    void insert(int key, int value) {
        Node* node = new Node(key, value);
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }

    bool remove(int key) {
        Node* curr = head;
        while (curr) {
            if (curr->key == key) {
                if (curr->prev) curr->prev->next = curr->next;
                if (curr->next) curr->next->prev = curr->prev;
                if (curr == head) head = curr->next;
                delete curr;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    Node* find(int key) {
        Node* curr = head;
        while (curr) {
            if (curr->key == key) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    void clear() {
        Node* curr = head;
        while (curr) {
            Node* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        head = nullptr;
    }
};

// Hash Table with chaining and dynamic resizing
class HashTable {
private:
    LinkedList** table;
    int capacity;
    int size;
    HashFunc hashFunc;

    // Multiplication method
    static int multiplicationHash(int key, int cap) {
        float A = 0.6180339887; // golden ratio part
        return int(cap * fmod(key * A, 1));
    }

    // Division method
    static int divisionHash(int key, int cap) {
        return key % cap;
    }

    void rehash(int newCapacity) {
        LinkedList** oldTable = table;
        int oldCapacity = capacity;

        capacity = newCapacity;
        table = new LinkedList*[capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i] = new LinkedList();
        }

        size = 0;
        for (int i = 0; i < oldCapacity; ++i) {
            Node* curr = oldTable[i]->head;
            while (curr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
            oldTable[i]->clear();
            delete oldTable[i];
        }
        delete[] oldTable;
    }

    void resizeIfNeeded() {
        float load = (float)size / capacity;
        if (load > LOAD_FACTOR_GROW) {
            rehash(capacity * 2);
        } else if (load < LOAD_FACTOR_SHRINK && capacity > 2) {
            rehash(capacity / 2);
        }
    }

public:
    HashTable(int initialCap = 4, bool useMultiplication = true) {
        capacity = initialCap;
        size = 0;
        table = new LinkedList*[capacity];
        for (int i = 0; i < capacity; ++i)
            table[i] = new LinkedList();

        hashFunc = useMultiplication ? multiplicationHash : divisionHash;
    }

    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            table[i]->clear();
            delete table[i];
        }
        delete[] table;
    }

    void insert(int key, int value) {
        int idx = hashFunc(key, capacity);
        Node* node = table[idx]->find(key);
        if (node) {
            node->value = value;
        } else {
            table[idx]->insert(key, value);
            size++;
            resizeIfNeeded();
        }
    }

    bool remove(int key) {
        int idx = hashFunc(key, capacity);
        bool removed = table[idx]->remove(key);
        if (removed) {
            size--;
            resizeIfNeeded();
        }
        return removed;
    }

    bool search(int key, int& valueOut) {
        int idx = hashFunc(key, capacity);
        Node* node = table[idx]->find(key);
        if (node) {
            valueOut = node->value;
            return true;
        }
        return false;
    }

    void display() {
        for (int i = 0; i < capacity; ++i) {
            cout << "Bucket " << i << ": ";
            Node* curr = table[i]->head;
            while (curr) {
                cout << "[" << curr->key << ":" << curr->value << "] <-> ";
                curr = curr->next;
            }
            cout << "NULL\n";
        }
    }

    int getSize() { return size; }
    int getCapacity() { return capacity; }
};
int main() {
    HashTable ht(4, true); // Start with multiplication method

    ht.insert(10, 100);
    ht.insert(20, 200);
    ht.insert(30, 300);
    ht.insert(40, 400);

    ht.display();

    ht.remove(20);
    ht.display();

    int val;
    if (ht.search(10, val)) {
        cout << "Found 10 -> " << val << endl;
    }

    return 0;
}
