#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using Clock = std::chrono::steady_clock;

vector<int> standard_vector;
string standard_filename = "";

int data_amount;

vector<string> entries;
string entries_filename = "";

vector<int> sizes;

struct Entry {
    string type;
    double time;
    int memory;
    int size;
    
    Entry(string type, double time, int memory, int size){
        this -> type = type;
        this -> time = time;
        this -> memory = memory;
        this -> size = size;
    }

    string toString(){
        return( type + "," + to_string(time) + "," + to_string(memory) + "," + to_string(size));
    }

};

bool load_data(string standard_filename, int n) {
    ifstream fileInput;

    fileInput.open(standard_filename, ios::in);
    if(fileInput.fail()) return false;
    
    string line;

    for(int i = 0; i < n; i++){
        fileInput >> line;
        if(!fileInput.eof()) standard_vector.push_back(stoi(line));
        else return false;
    }
    fileInput.close();
    return true;
}

bool load_config() {
    ifstream fileInput;
    fileInput.open("config_list.ini", ios::in);
    if(fileInput.fail()) {
        fileInput.close();
        return false;
    }
    vector<string> row;
    string line;

    fileInput >> data_amount;
    fileInput >> standard_filename;
    fileInput >> entries_filename;

    for(int i = 0; !fileInput.eof(); i++){
        int buf;
        fileInput >> buf;
        sizes.push_back(buf);
    };
    fileInput.close();
    return true;
}

void save_entries(string entries_file_name) {
    fstream fileOutput;
    fileOutput.open(entries_file_name, ios::out);
    fileOutput << "sort type,time,memory,size" << endl;
    for(int i = 0; i < entries.size(); i++) fileOutput << entries[i] << endl;
    fileOutput.close();
}

// Wezel, sluzacy jako reprezentacja listy jednokierunkowej
struct Node {
    int value;
    struct Node* next;
};

// Wstawianie nowego wezla
void push(struct Node** head, int value) {
    struct Node* node = new Node;
    node -> value = value;
    node -> next = (*head);
    (*head) = node;
}

// Otrzymanie ostatniego wezla
struct Node* getTail(struct Node* node) {
    while(node != NULL && node -> next != NULL) node = node -> next;
    return node;
}

// Utworz partycje i ustaw ja jako ostatni wezel
struct Node* partition(struct Node* head, struct Node* end, struct Node** newHead, struct Node** newEnd) {
    struct Node *pivot = end;                               // Ustaw partcje jako ostatni wezel
    struct Node *prev = NULL, *cur = head, *tail = pivot;   // Utworz zmienne pomocnicze

    while(cur != pivot){
        if(cur -> value < pivot -> value){                  // Jezeli wezel ma mniejsza wartosc niz partycja
            if((*newHead) == NULL) (*newHead) = cur;
            prev = cur;                                     // Ustaw partcje jako nowa glowe
            cur = cur -> next;
        } else {                                            // Jezeli wezel ma wieksza wartosc niz partycja
            if (prev) prev -> next = cur -> next;           // Przenies wezel jako kolejny po ogonie
            struct Node *tmp = cur -> next;
            tail -> next = cur;
            tail = cur;
            cur = tmp;
        }
    }
    if ((*newHead) == NULL) (*newHead) = pivot;             // Jezeli partycja jest najmniejszym elementem, staje sie glowa
    (*newEnd) = tail;                                       // Zaktualizuj ogon

    return pivot;
}

// Podfunkcja sortowania, gdzie wykonuje sie sortowanie wszystkich elementow poza ostatnim
struct Node* quickSortRecur(struct Node* head, struct Node* end, int* mem) {
    if (!head || head == end) return head;
    Node *newHead = NULL, *newEnd = NULL;
    *mem += sizeof(newHead) + sizeof(newEnd);
    struct Node* pivot = partition(head, end, &newHead, &newEnd);   // Podziel liste
    *mem += sizeof(pivot);

    if(newHead != pivot){                                           // Jezeli partycja nie jest najmniejszym elementem
        struct Node *tmp = newHead;                                 // Ustaw wezel przed partycja jako NULL
        while (tmp -> next != pivot) tmp = tmp -> next;             
        tmp -> next = NULL;
        newHead = quickSortRecur(newHead, tmp, mem);                // Wywolaj rekurencyjnie przed partycja

        tmp = getTail(newHead);                                     // Ustaw nastepny wezel ostatniego wezla jako partycje
        tmp -> next = pivot;
    }
    pivot -> next = quickSortRecur(pivot -> next, newEnd, mem);

    return newHead;
}

// Glowna funkcja sortowania
void quickSort(struct Node** headRef, int* mem){
    (*headRef) = quickSortRecur(*headRef, getTail(*headRef), mem);
    return;
}

int main() {
    load_config();
    load_data(standard_filename, data_amount);

    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed;

    int *mem = new int;

    for(int i = 0; i < sizes.size(); i++){
        *mem = 0;                               // Wyczysc zmienna zliczajaca wykorzystana pamiec

        struct Node* node = NULL;               // Utworz wezly i zapelnij je danymi
        for(int k = 0; k < sizes[i]; k++) {
            push(&node, standard_vector[i]);
        }

        t_1 = Clock::now();                     // Rozpocznij zegar
        quickSort(&node, mem);                  // Posortuj elementy listy
        t_2 = Clock::now();                     // Zatrzymaj zegar

        t_elapsed = chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
            
        Entry entry = Entry("Quicksort", t_elapsed.count(), *mem,  sizes[i]);
        entries.push_back(entry.toString());    // Zapisz wynik

        cout << "Finished Quicksort for " << sizes[i] << " in " << t_elapsed.count() << "s." << endl;

        delete node;
        delete mem;
    }

    string final_entries_filename = "list_quicksort_" + entries_filename;
    save_entries(final_entries_filename);       // Zapisz wyniki do pliku
    entries.clear();

    return 0;
}