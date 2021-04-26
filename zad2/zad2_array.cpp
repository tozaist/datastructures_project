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

vector<int> arrSizes;

struct Entry{
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

bool load_data(string standard_filename, int n){
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

bool load_config(){
    ifstream fileInput;
    fileInput.open("config.ini", ios::in);
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
        arrSizes.push_back(buf);
    };
    fileInput.close();
    return true;
}

void save_entries(string entries_file_name){
    fstream fileOutput;
    fileOutput.open(entries_file_name, ios::out);
    fileOutput << "sort type,time,memory,size" << endl;
    for(int i = 0; i < entries.size(); i++) fileOutput << entries[i] << endl;
    fileOutput.close();
}

// Wypelnij tablice danymi
int* fillArray(int size){                           
    int* arr = new int[size];
    for(int i = 0; i < size; i++) arr[i] = standard_vector[i];
    return arr;
}

// Zamien wartosci pod adresami
void swap(int *xp, int *yp) {                       
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

/* Utworz partycje, umiesc ja w odpowiednim miejscu
posortowanej tablicy i umiesc wartosci mniejsze niz
partycja na lewo a wieksze na prawo */
int partition(int arr[], int l, int h) {        
    int pivot = arr[h];                 // Ustaw partycje na koncu
    int i = l - 1;                      // Klucz mniejszego elementu i pozycja odnalezionej juz partycji
    for (int j = l; j <= h - 1; j++) {
        if (arr[j] <= pivot) {          // Jezeli badany element jest mniejszy niz partycja
            i++;                        // Zwieksz klucz mniejszego elementu
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[h]);
    return (i + 1);
}

/* Glowna funkcja sortowania szybkiego,
przyjmujaca jako argumenty tablice do posortowania,
jej pierwszy i ostatni klucz */
void quickSort(int arr[], int l, int h, int* mem) {
    if (l < h) {
        int pivot = partition(arr, l, h);   // Odnajdz klucz partycji
        *mem += sizeof(pivot);
        quickSort(arr, l, pivot - 1, mem);  // Posortuj elementy przed partycja
        quickSort(arr, pivot + 1, h, mem);  // Posortuj elementy po partycji
    }
}

/* Funkcja pomocnicza sortowania przez zliczanie,
zwraca najmniejszy element w tablicy */
int getMin(int arr[], int size) {
    int min = arr[0];                       // Ustaw min jako pierwszy element tablicy
    for(int i = 0; i < size; i++) {         // Przejdz po wszystkich elementach tablicy
        if(arr[i] < min) min = arr[i];      // Jezeli ktorys jest mniejszy, ustaw go jako min
    }
    return min;                             // Zwroc min
}

/* Funkcja pomocnicza sortowania przez zliczanie,
zwraca najwiekszy element w tablicy */
int getMax(int arr[], int size) {
    int max = arr[0];                       // Ustaw max jako pierwszy element tablicy
    for(int i = 0; i < size; i++) {         // Przejdz po wszystkich elementach tablicy
        if(arr[i] > max) max = arr[i];      // Jezeli ktorys jest wiekszy, ustaw go jako max
    }
    return max;                             // Zwroc max
}

/* Glowna funkcja sortowania przez zliczanie,
przyjmujaca jako argumenty tablice do posortowania i jej rozmiar */
void countingSort(int arr[], int size, int* mem) {
    int* result = new int[size];            // Utworz tablice wynikowa
    *mem += size * sizeof(result[0]);

    int min = getMin(arr, size);            // Ustal najmniejszy element tablicy
    *mem += sizeof(min);
    int max = getMax(arr, size);            // Ustal najwiekszy element tablicy
    *mem += sizeof(max);

    int* count = new int[max - min + 1];    // Utworz tablice zliczeniowa
    *mem += (max - min + 1) * sizeof(count[0]);
    
    memset(count, 0, (max - min + 1) * sizeof(int));

    for(int i = 0; i < size; i++) {
        count[arr[i] - min]++;              // Dla kazdego elementu w tablicy zrodlowej
                                            // inkrementuj jego adres w tablicy zliczeniowej
    }

    for(int i = 1; i < (max - min + 1); i++) {
        count[i] += count[i - 1];           // Sumuj kolejne wartosci tablicy zliczeniowej
    }

    for(int i = size - 1; i >= 0; i--) {
        count[arr[i] - min]--;                  // Zredukuj wartosc tablicy zliczeniowej
        result[count[arr[i] - min]] = arr[i];   // Przepisz wartosc pobrana z tablicy zliczeniowej
                                                // do tablicy wynikowej
    }

    memcpy(arr, result, size * sizeof(int));    // Zastap tablice zrodlowa posortowana tablica
    delete result;
    delete count;
}

int main() {
    load_config();
    load_data(standard_filename, data_amount);

    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed;

    int *mem = new int;

    for(int i = 0; i < arrSizes.size(); i++){
        *mem = 0;                                   // Wyczysc zmienna zliczajaca wykorzystana pamiec
        int *arr = fillArray(arrSizes[i]);          // Utworz tablice o podanym rozmiarze i wypelnij ja danymi

        t_1 = Clock::now();                         // Rozpocznij zegar
        quickSort(arr, 0, arrSizes[i] - 1, mem);    // Posortuj elementy tablicy
        t_2 = Clock::now();                         // Zatrzymaj zegar
        t_elapsed = chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

        Entry entry = Entry("Quicksort", t_elapsed.count(), *mem,  arrSizes[i]);
        entries.push_back(entry.toString());        // Zapisz wynik

        cout << "Finished Quicksort for " << arrSizes[i] << " in " << t_elapsed.count() << "s. Used " << *mem << " space." << endl;
        delete arr;
    }

    string final_entries_filename = "array_quicksort_" + entries_filename;
    save_entries(final_entries_filename);
    entries.clear();

    for(int i = 0; i < arrSizes.size(); i++){
        *mem = 0;                                   // Wyczysc zmienna zliczajaca wykorzystana pamiec
        int *arr = fillArray(arrSizes[i]);          // Utworz tablice o podanym rozmiarze i wypelnij ja danymi

        t_1 = Clock::now();                         // Rozpocznij zegar
        countingSort(arr, arrSizes[i] - 1, mem);    // Posortuj elementy tablicy
        t_2 = Clock::now();                         // Zatrzymaj zegar
        t_elapsed = chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

        Entry entry = Entry("CountingSort", t_elapsed.count(), *mem, arrSizes[i]);
        entries.push_back(entry.toString());        // Zapisz wynik

        cout << "Finished CountingSort for " << arrSizes[i] << " in " << t_elapsed.count() << "s. Used " << *mem << " space." << endl;
        delete arr;
    }

    final_entries_filename = "array_countsort_" + entries_filename;
    save_entries(final_entries_filename);           // Zapisz wyniki do pliku
    entries.clear();

    delete mem;

    return 0;
}