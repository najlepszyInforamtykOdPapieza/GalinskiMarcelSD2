#include <iostream>
#include <stdexcept>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
class DynamicArray {
private:
    int* arr;           // Wskaźnik
    int capacity;       // Pojemność
    int current_size;   // ileMamy

    // Podwajamy Pojemność
    void resize() {
        capacity *= 2;                  
        int* new_arr = new int[capacity]; // Stawiamy nową tablicę

        // Stara tablica[] = nowa tablica[]
        for (int i = 0; i < current_size; ++i) {
            new_arr[i] = arr[i];
        }

        delete[] arr; // Stara wylatuje za okno
        arr = new_arr; // Podmiana wskaźnika
    }

public:
    // Konstruktor pojemość = 2
    DynamicArray(int precapacity = 2) {
        capacity = precapacity;
        current_size = 0;
        arr = new int[capacity];
    }

    // Destruktor 
    ~DynamicArray() {
        delete[] arr;
    }


    // ***** MAGICZNE METODY ******
    
    // Dodaje 
    void addFront(int value) {
        //Zachowuje sie jak 2 kolejne funkcje
        if (current_size == capacity) {
            resize();
        }
        
 
        for (int i = current_size; i > 0; --i) {
            arr[i] = arr[i - 1];
        }
        
      
        arr[0] = value;
        
        
        current_size++;
    }


    // Tez dodaje
    void addBack(int value) {
        // 1. Sprawdzamy, czy jest miejsce - jeśli nie, podwajamy pojemność
        if (current_size == capacity) {
            resize();
        }
        
        // 2. Dodajemy element na pierwsze wolne miejsce
        arr[current_size] = value;
        
        // 3. Zwiększamy licznik elementów
        current_size++;
    }


    // A to dodaje i sprawdza czy ktos nie wpisal blednie
    void addAt(int index, int value) {
        // Sprawdzamy czy ktoś się nie pomylił
        if (index < 0 || index > current_size) {
            throw std::out_of_range("Blad: Indeks poza zakresem tablicy!");
        }

        // Mamy miejsce Ziutek?
        if (current_size == capacity) {
            resize();
        }

        // Jak mamy to panowie na prawo krok w prawo
        for (int i = current_size; i > index; --i) {
            arr[i] = arr[i - 1];
        }

        // A ja sie tu wpycham
        arr[index] = value;

        current_size++;
    }

   // Usuwa
    void removeFront() {
        if (current_size == 0) {
            throw std::out_of_range("Blad: Tablica jest pusta, nie mozna usunac elementu!");
        }
        
        // wszystko nadpisujemy i usuwamy ostatni
        for (int i = 0; i < current_size - 1; ++i) {
            arr[i] = arr[i + 1];
        }
        
        current_size--;
    }
    void removeBack() {
        // Najpierw sprawdzamy, czy w ogóle jest co usuwać
        if (current_size == 0) {
            throw std::out_of_range("Blad: Tablica jest pusta, nie mozna usunac elementu!");
        }
        
        // Zmniejszamy licznik - element logicznie znika
        // Sprawdzic czy wstawienie 0 coś przyśpieszy ////
        current_size--;
    }
    void removeAt(int index) {
        // Sprawdzamy czy mamy co usuwac
        if (index < 0 || index >= current_size) {
            throw std::out_of_range("Blad: Indeks poza zakresem tablicy!");
        }

        // Przesuwamy w lewo tylko elementy znajdujące się za usuwanym indeksem
        for (int i = index; i < current_size - 1; ++i) {
            arr[i] = arr[i + 1];
        }

        current_size--;
    }

    // Wyszukuje
    bool search(int value) {
        for (int i = 0; i < current_size; ++i) {
            if (arr[i] == value) {
                return true; // Znaleziono!
            }
        }
        return false; // Przeszukaliśmy całą tablicę i nie ma tej liczby
    }

    // Szybki podgląd
    void print() {
        std::cout << "[ ";
        for (int i = 0; i < current_size; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << "] (size: " << current_size << ", capacity: " << capacity << ")\n";
    }
};
using namespace std;
using namespace std::chrono;

// Parametry 
const int START_SIZE = 10000;
const int MAX_SIZE = 100000;
const int STEP = 10000;
const int MEASUREMENTS = 1000; 



int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000000);


    cout << fixed << setprecision(0);

    cout << "Rozmiar(N);AddFront(ns);Search(ns)\n";

    for (int N = START_SIZE; N <= MAX_SIZE; N += STEP) {
        

        vector<DynamicArray> arrays(MEASUREMENTS);
        
    
        for (int i = 0; i < MEASUREMENTS; ++i) {
            for (int j = 0; j < N; ++j) {
                arrays[i].addBack(dist(gen)); 
            }
        }

   
        auto start_add_front = high_resolution_clock::now();
        
        for (int i = 0; i < MEASUREMENTS; ++i) {
            arrays[i].addFront(dist(gen)); 
        }
        
        auto end_add_front = high_resolution_clock::now();
        double avg_add_front = (double)duration_cast<nanoseconds>(end_add_front - start_add_front).count() / MEASUREMENTS;


        int random_value_to_search = dist(gen); 
        
        auto start_search = high_resolution_clock::now();
        
        for (int i = 0; i < MEASUREMENTS; ++i) {
            arrays[i].search(random_value_to_search); 
        }
        
        auto end_search = high_resolution_clock::now();
        double avg_search = (double)duration_cast<nanoseconds>(end_search - start_search).count() / MEASUREMENTS;

  
        cout << N << ";" << avg_add_front << ";" << avg_search << "\n";
    }

    return 0;
}