#include <iostream>
#include <stdexcept>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>

// struktura wezła
struct Node {
    int data;       // Przechowywana wartość
    Node* next;     // Wskaźnik na następny element

    // Konstruktor węzła
    Node(int val) {
        data = val;
        next = nullptr; 
    }
};

class SinglyLinkedList {
private:
    Node* head;     // Wskaźnik HEAD
    Node* tail;     // Wskaźnik TAIL

public:
    SinglyLinkedList() {
        head = nullptr;
        tail = nullptr;
    }

    ~SinglyLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next_node = current->next;
            delete current;
            current = next_node;
        }
    }


    // Dodaje
   void addFront(int value) {
        Node* newNode = new Node(value); // NOWY WĘZEŁ

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            // Zamiana Głowy
            newNode->next = head;
            head = newNode;
        }
    }

    void addBack(int value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            // Zamiana ogona
            tail->next = newNode;
            tail = newNode;
        }
    }
    void addAt(int index, int value) {
        if (index < 0) {
            throw std::out_of_range("Blad: Indeks nie moze byc ujemny!");
        }

        // wstawianie na front(HEADA) to to samo co adFront
        if (index == 0) {
            addFront(value);
            return;
        }

        // szyukamy poprzedniego elementu
        Node* current = head;
        for (int i = 0; i < index - 1; ++i) {
            if (current == nullptr) {
                throw std::out_of_range("Blad: Indeks poza zakresem!");
            }
            current = current->next;
        }

        // Sprawdzamy czy mamy taki element
        if (current == nullptr) {
            throw std::out_of_range("Blad: Indeks poza zakresem!");
        }

        // Tworzymy element
        Node* newNode = new Node(value);
        newNode->next = current->next; 
        current->next = newNode;       

        // jeśli wstawiamy TAILA musimy uaktualnic wstaznik TAIL
        if (newNode->next == nullptr) {
            tail = newNode;
        }
    }

    // Usuwa
    void removeFront() {
        if (head == nullptr) {
            throw std::out_of_range("Blad: Lista jest pusta!");
        }

        Node* temp = head;       
        head = head->next;       
        delete temp;             

        // Czyscimy węzeł jeśli lista jest pusta
        if (head == nullptr) {
            tail = nullptr;
        }
    }
    void removeBack() {
        if (head == nullptr) {
            throw std::out_of_range("Blad: Lista jest pusta!");
        }

        // Jeśli w liście jest tylko jeden element
        if (head == tail) {
            delete head;
            head = nullptr;
            tail = nullptr;
            return;
        }

        // Szukamy PRZEDOSTATNIEGO elementu
        Node* current = head;
        while (current->next != tail) {
            current = current->next;
        }

        // current to teraz przedostatni element.
        delete tail;             
        tail = current;          
        tail->next = nullptr;    
    }
   void removeAt(int index) {
        if (index < 0 || head == nullptr) {
            throw std::out_of_range("Blad: Nieprawidlowy indeks lub lista jest pusta!");
        }

        // Patent
        if (index == 0) {
            removeFront();
            return;
        }

        // Szukamy elementu poprzedniego elementu
        Node* current = head;
        for (int i = 0; i < index - 1; ++i) {
            if (current->next == nullptr) {
                throw std::out_of_range("Blad: Indeks poza zakresem!");
            }
            current = current->next;
        }

        Node* nodeToDelete = current->next; // Element do usunięcia
        if (nodeToDelete == nullptr) {
            throw std::out_of_range("Blad: Indeks poza zakresem!");
        }

        // Przepinamy wskaźnik, omijając usuwany element
        current->next = nodeToDelete->next;

        // Jeśli usuwaliśmy ostatni element, musimy zaktualizować ogon
        if (nodeToDelete == tail) {
            tail = current;
        }

        delete nodeToDelete; 
    }

    bool search(int value) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                return true; // Znaleziony
            }
            current = current->next;
        }
        return false; // Nie znaleziony
    }

    // Podglad
    void print() {
        Node* current = head;
        std::cout << "HEAD -> ";
        while (current != nullptr) {
            std::cout << "[" << current->data << "] -> ";
            current = current->next;
        }
        std::cout << "NULL\n";
    }
};
using namespace std;
using namespace std::chrono;

// Parametry 
const int START_SIZE = 10000;
const int MAX_SIZE = 100000;
const int STEP = 10000;
const int MEASUREMENTS = 1000; 


#include <iostream>
#include <chrono>
#include <random>
#include <vector>


int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000000);

    cout << fixed << setprecision(0);

    cout << "Rozmiar(N);RemoveBack(ns);RemoveAt(ns)\n";

    for (int N = START_SIZE; N <= MAX_SIZE; N += STEP) {
        

        vector<SinglyLinkedList> arrays(MEASUREMENTS);
        
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