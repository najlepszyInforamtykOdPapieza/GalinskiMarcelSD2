#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
using namespace std;

struct Element {
    int value;
    int priority;
    long long insert_id; 
};

struct Node {
    Element data;
    Node* prev;
    Node* next;
    
    Node(Element d) : data(d), prev(nullptr), next(nullptr) {}
};

class KopiecMax {
private:
    Element* tablica;       // Dynamiczna tablica na dane
    int rozmiar;            // Aktualna liczba elementów w kopcu
    int pojemnosc;          // Maksymalny rozmiar tablicy
    long long global_id;    // Licznik, który rośnie z każdym dodanym elementem!

    // Ustawia elementy wedlug priorytetu
    bool czyWyzszyPriorytet(const Element& dziecko, const Element& rodzic) {
        // 1. Zwykły przypadek - wyższy priorytet idzie do góry
        if (dziecko.priority > rodzic.priority) {
            return true;
        }
        // Gdy jest remis używamy FIFO
        else if (dziecko.priority == rodzic.priority) {
            return dziecko.insert_id < rodzic.insert_id;
        }
        
        return false; 
    }

    // Funkcja naprawiająca kopiec po dodaniu na koniec
    void heapifyUp(int index) {
        while (index > 0) {
            int parentIndex = (index - 1) / 2; // wzór na rodzica

            if (czyWyzszyPriorytet(tablica[index], tablica[parentIndex])) {
                swap(tablica[index], tablica[parentIndex]);
                
                // wchodzimy po kopcu do góry
                index = parentIndex;
            } else {
                break; 
            }
        }
    }

    // Funkcja powiększająca tablicę dynamiczną w razie potrzeby
    void powiekszTablice() {
        pojemnosc *= 2;
        Element* nowaTablica = new Element[pojemnosc];
        for (int i = 0; i < rozmiar; i++) {
            nowaTablica[i] = tablica[i];
        }
        delete[] tablica;
        tablica = nowaTablica;
    }

    void heapifyDown(int index) {
        int najsilniejszy = index; // Zakładamy, że obecny węzeł jest na razie najsilniejszy
        int leweDziecko = 2 * index + 1;
        int praweDziecko = 2 * index + 2;

        // Sprawdzamy, czy lewe dziecko istnieje i czy jest silniejsze od obecnego lidera
        if (leweDziecko < rozmiar && czyWyzszyPriorytet(tablica[leweDziecko], tablica[najsilniejszy])) {
            najsilniejszy = leweDziecko;
        }

        // Sprawdzamy, czy prawe dziecko istnieje i czy jest silniejsze od obecnego lidera
        if (praweDziecko < rozmiar && czyWyzszyPriorytet(tablica[praweDziecko], tablica[najsilniejszy])) {
            najsilniejszy = praweDziecko;
        }

        // Jeśli któryś z potomków okazał się silniejszy, musimy dokonać zamiany
        if (najsilniejszy != index) {
            swap(tablica[index], tablica[najsilniejszy]);
            heapifyDown(najsilniejszy); // Rekurencyjnie naprawiamy dalej w dół
        }
    }
public:
    // Konstruktor
    KopiecMax(int poczatkowaPojemnosc = 10) {
        pojemnosc = poczatkowaPojemnosc;
        rozmiar = 0;
        global_id = 0; // Zaczynamy odliczanie ID
        tablica = new Element[pojemnosc];
    }

    // Destruktor
    ~KopiecMax() {
        delete[] tablica;
    }

    //  INSERT
    void insert(int wartosc, int priorytet) {
        if (rozmiar == pojemnosc) {
            powiekszTablice(); 
        }

        // 1. Tworzymy nowy element i wrzucamy na sam koniec tablicy
        Element nowyElement;
        nowyElement.value = wartosc;
        nowyElement.priority = priorytet;
        nowyElement.insert_id = global_id++; 

        tablica[rozmiar] = nowyElement;

        // 2. Naprawiamy strukturę kopca w górę
        heapifyUp(rozmiar);

        rozmiar++;
    }

    // extract-max()
   
    Element extractMax() {
        if (rozmiar <= 0) {
            throw out_of_range("Kopiec jest pusty!");
        }

        if (rozmiar == 1) {
            rozmiar--;
            return tablica[0];
        }

        // 1. Zapisujemy element z samego szczytu (największy priorytet/najstarszy)
        Element root = tablica[0];

        // 2. Ostatni element w tablicy wskakuje na miejsce korzenia
        tablica[0] = tablica[rozmiar - 1];
        rozmiar--;

        // 3. Naprawiamy kopiec
        heapifyDown(0);

        return root;
    }

    // peek() / find-max()
    Element peek() {
        if (rozmiar <= 0) {
            throw out_of_range("Kopiec jest pusty!");
        }
        return tablica[0]; // Złożoność idealne O(1) - korzeń zawsze ma najwyższy priorytet
    }

    // return-size() - Zwraca aktualny rozmiar kolejki

    int returnSize() {
        return rozmiar;
    }

    // modify-key(e, p) - Zmiana priorytetu 

    void modifyKey(int wartoscElementu, int nowyPriorytet) {
        // W standardowym kopcu nie wiemy, gdzie leży element, więc musimy go znaleźć (O(n))
        int index = -1;
        for (int i = 0; i < rozmiar; i++) {
            if (tablica[i].value == wartoscElementu) {
                index = i;
                break;
            }
        }

        if (index == -1) return; // Nie znaleziono elementu

        int staryPriorytet = tablica[index].priority;
        tablica[index].priority = nowyPriorytet;

        // Jeśli nowy priorytet jest większy -> element staje się silniejszy -> idzie w górę
        if (nowyPriorytet > staryPriorytet) {
            heapifyUp(index);
        }
        // Jeśli nowy priorytet jest mniejszy -> element słabnie -> opada w dół
        else if (nowyPriorytet < staryPriorytet) {
            heapifyDown(index);
        }
    
    }
};

class ListaDwukierunkowa {
private:
    Node* head;
    Node* tail;
    int rozmiar;
    long long global_id; // Używamy dla spójności z kopcem, choć lista naturalnie trzyma kolejność

public:
    ListaDwukierunkowa() : head(nullptr), tail(nullptr), rozmiar(0), global_id(0) {}

    ~ListaDwukierunkowa() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    // insert(e, p) wspiuje warość i priotytet
    void insert(int wartosc, int priorytet) {
        Element nowyElement;
        nowyElement.value = wartosc;
        nowyElement.priority = priorytet;
        nowyElement.insert_id = global_id++;

        Node* nowyWezel = new Node(nowyElement);

        if (rozmiar == 0) {
            head = tail = nowyWezel;
        } else {
            tail->next = nowyWezel;
            nowyWezel->prev = tail;
            tail = nowyWezel;
        }
        rozmiar++;
    }

    // extract-max() usuwa najwiekszy priorytet

    Element extractMax() {
        if (rozmiar == 0) {
            throw out_of_range("Lista jest pusta!");
        }

        Node* maxNode = head;
        Node* current = head->next;

        // 1. Szukamy węzła z największym priorytetem
        while (current != nullptr) {
            // Używamy TYLKO znaku '>', więc przy remisie zostaje starszy element (FIFO)
            if (current->data.priority > maxNode->data.priority) {
                maxNode = current;
            }
            current = current->next;
        }

        Element rootData = maxNode->data; // Zapisujemy dane do zwrócenia

        // 2. Wypinamy znaleziony węzeł z listy (łatamy dziurę)
        if (maxNode->prev != nullptr) {
            maxNode->prev->next = maxNode->next; // Ktoś był przed nami
        } else {
            head = maxNode->next; // Byliśmy głową
        }

        if (maxNode->next != nullptr) {
            maxNode->next->prev = maxNode->prev; // Ktoś był za nami
        } else {
            tail = maxNode->prev; // Byliśmy ogonem
        }

        // 3. Usuwamy z pamięci i zmniejszamy rozmiar
        delete maxNode;
        rozmiar--;

        return rootData;
    }

    // peek() / find-max()  znajduje najwiekszy priorytet

    Element peek() {
        if (rozmiar == 0) {
            throw out_of_range("Lista jest pusta!");
        }

        Node* maxNode = head;
        Node* current = head->next;

        while (current != nullptr) {
            if (current->data.priority > maxNode->data.priority) {
                maxNode = current;
            }
            current = current->next;
        }

        return maxNode->data;
    }

    // return-size() zwraca rozmiar

    int returnSize() {
        return rozmiar;
    }

    // modify-key(e, p)  Szukamy i zmieniamy.

    void modifyKey(int wartoscElementu, int nowyPriorytet) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.value == wartoscElementu) {
                current->data.priority = nowyPriorytet;
                // Lista pozostaje nieposortowana, więc niczego nie musimy przestawiać!
                return; 
            }
            current = current->next;
        }
    }
};

int main() {

    const int N_MIN = 10000;       
    const int N_MAX = 100000;      
    const int KROK = 10000;        
    const int PROBKI = 100;        // 100 kopii (seedów) na każde N

    // Seed żeby wyniki były wiarygodne
    unsigned int test_seed = 12345; 
    mt19937 gen(test_seed);
    
    uniform_int_distribution<> dystrybucjaPriorytetu(1, 500000); 
    uniform_int_distribution<> dystrybucjaWartosci(1, 10000);


    cout << "--- BADANIE OPERACJI: RETURN-SIZE ---" << endl;
    cout << "Parametry: Probki = " << PROBKI << " na kazde N" << endl;
    cout << "N;Kopiec_Max(us);Lista_Dwukierunkowa(us)" << endl;

    for (int N = N_MIN; N <= N_MAX; N += KROK) {
        
        double sumaCzasKopiec = 0.0;
        double sumaCzasLista = 0.0;

        for (int p = 0; p < PROBKI; p++) {
            
            // A. Generowanie WZORCOWEJ bazy N elementów
            vector<Element> daneWzorcowe;
            daneWzorcowe.reserve(N);
            for (int i = 0; i < N; i++) {
                Element el;
                el.value = dystrybucjaWartosci(gen);
                el.priority = dystrybucjaPriorytetu(gen);
                daneWzorcowe.push_back(el);
            }

            // B. Zapełnianie struktur do rozmiaru N (POZA STOPEREM)
            KopiecMax kopiec(N); 
            ListaDwukierunkowa lista;

            for (int i = 0; i < N; i++) {
                kopiec.insert(daneWzorcowe[i].value, daneWzorcowe[i].priority);
                lista.insert(daneWzorcowe[i].value, daneWzorcowe[i].priority);
            }


            // POMIAR 1: KOPIEC
            auto startKopiec = chrono::high_resolution_clock::now();
            volatile int rozmiarKopca = kopiec.returnSize(); 
            auto endKopiec = chrono::high_resolution_clock::now();
            
            sumaCzasKopiec += chrono::duration<double, micro>(endKopiec - startKopiec).count();

            // POMIAR 2: LISTA
            auto startLista = chrono::high_resolution_clock::now();
            volatile int rozmiarListy = lista.returnSize();
            auto endLista = chrono::high_resolution_clock::now();
            
            sumaCzasLista += chrono::duration<double, micro>(endLista - startLista).count();
        }

        // Uśredniamy wyniki ze 100 próbek
        double sredniaKopiec = sumaCzasKopiec / PROBKI;
        double sredniaLista = sumaCzasLista / PROBKI;
        
        cout << N << ";" << sredniaKopiec << ";" << sredniaLista << endl;
    }

    return 0;
}