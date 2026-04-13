#include <iostream>
#include <stdexcept>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>

// Struktura węzła listy dwukierunkowej (ma prev i next)
struct DNode {
    int data;
    DNode* next;
    DNode* prev;

    DNode(int val) {
        data = val;
        next = nullptr;
        prev = nullptr;
    }
};

class DoublyLinkedList {
private:
    DNode* head;
    DNode* tail;

public:
    // Konstruktor
    DoublyLinkedList() {
        head = nullptr;
        tail = nullptr;
    }

    // Destruktor
    ~DoublyLinkedList() {
        DNode* current = head;
        while (current != nullptr) {
            DNode* next_node = current->next;
            delete current;
            current = next_node;
        }
    }


    void addFront(int value) {
        DNode* newNode = new DNode(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
    }

    void addBack(int value) {
        DNode* newNode = new DNode(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void addAt(int index, int value) {
        if (index < 0) throw std::out_of_range("Indeks nie moze byc ujemny!");
        if (index == 0) {
            addFront(value);
            return;
        }

        DNode* current = head;
        for (int i = 0; i < index - 1; ++i) {
            if (current == nullptr) throw std::out_of_range("Indeks poza zakresem!");
            current = current->next;
        }

        if (current == nullptr) throw std::out_of_range("Indeks poza zakresem!");

        DNode* newNode = new DNode(value);
        newNode->next = current->next;
        newNode->prev = current;

        if (current->next != nullptr) {
            current->next->prev = newNode;
        } else {
            tail = newNode; // Jeśli dodajemy na sam koniec
        }
        current->next = newNode;
    }


    void removeFront() {
        if (head == nullptr) throw std::out_of_range("Lista jest pusta!");

        DNode* temp = head;
        head = head->next;

        if (head != nullptr) {
            head->prev = nullptr;
        } else {
            tail = nullptr; // Jeśli usunęliśmy jedyny element
        }
        delete temp;
    }

    void removeBack() {
        if (tail == nullptr) throw std::out_of_range("Lista jest pusta!");

        DNode* temp = tail;
        tail = tail->prev; // Cofamy ogon błyskawicznie dzięki wskaźnikowi prev

        if (tail != nullptr) {
            tail->next = nullptr;
        } else {
            head = nullptr; // Jeśli usunęliśmy jedyny element
        }
        delete temp;
    }

    void removeAt(int index) {
        if (index < 0 || head == nullptr) throw std::out_of_range("Zly indeks lub pusta lista!");
        if (index == 0) {
            removeFront();
            return;
        }

        DNode* current = head;
        for (int i = 0; i < index; ++i) {
            if (current == nullptr) throw std::out_of_range("Indeks poza zakresem!");
            current = current->next;
        }

        if (current == nullptr) throw std::out_of_range("Indeks poza zakresem!");

        // Przepinamy wskaźniki sąsiadów
        if (current->prev != nullptr) current->prev->next = current->next;
        if (current->next != nullptr) current->next->prev = current->prev;
        
        // Aktualizacja ogona, jeśli usuwaliśmy ostatni element
        if (current == tail) tail = current->prev;

        delete current;
    }


    bool search(int value) {
        DNode* current = head;
        while (current != nullptr) {
            if (current->data == value){
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void print() {
        DNode* current = head;
        std::cout << "NULL <-> ";
        while (current != nullptr) {
            std::cout << "[" << current->data << "] <-> ";
            current = current->next;
        }
        std::cout << "NULL\n";
    }
};
using namespace std;
using namespace std::chrono;

// Parametry naszych testów
const int START_SIZE = 10000;
const int MAX_SIZE = 100000;
const int STEP = 10000;
const int MEASUREMENTS = 1000; 

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip> 


using namespace std;
using namespace std::chrono;
int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000000);

 
    cout << fixed << setprecision(0);


    cout << "Rozmiar(N);RemoveBack(ns);RemoveAt(ns)\n";

    for (int N = START_SIZE; N <= MAX_SIZE; N += STEP) {
        
      
        vector<DoublyLinkedList> arrays(MEASUREMENTS);
        
        for (int i = 0; i < MEASUREMENTS; ++i) {
            for (int j = 0; j < N; ++j) {
                arrays[i].addBack(dist(gen)); 
            }
        }

  
        auto start_rem_back = high_resolution_clock::now();
        
        for (int i = 0; i < MEASUREMENTS; ++i) {
            arrays[i].removeBack(); 
        }
        
        auto end_rem_back = high_resolution_clock::now();
        double avg_rem_back = (double)duration_cast<nanoseconds>(end_rem_back - start_rem_back).count() / MEASUREMENTS;

     
        uniform_int_distribution<> index_dist(0, N - 2);
        int random_index = index_dist(gen);

        auto start_rem_at = high_resolution_clock::now();
        
        for (int i = 0; i < MEASUREMENTS; ++i) {
            arrays[i].removeAt(random_index); 
        }
        
        auto end_rem_at = high_resolution_clock::now();
        double avg_rem_at = (double)duration_cast<nanoseconds>(end_rem_at - start_rem_at).count() / MEASUREMENTS;

        cout << N << ";" << avg_rem_back << ";" << avg_rem_at << "\n";
    }

    return 0;
}