#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<chrono>
#include<cstdio>
#include<list>
#include<random>

using namespace std;
using Clock = std::chrono::steady_clock;


vector<int> database_vector;
string database_file_name = "";
int database_amount;

vector<string> entries;
string entries_file_name = "";

vector<int> instancesSize;

struct Entry{
    string type;
    string operation;
    int size;
    int repetitions;
    double time;
    
    Entry(string type, string operation, int size, int repetitions, double time){
        this -> type = type;
        this -> operation = operation;
        this -> size = size;
        this -> repetitions = repetitions;
        this -> time = time;
    }

    string toString(){
        return( type + "," + operation + "," + to_string(size) + "," + to_string(repetitions) + "," + to_string(time));
    }

};

bool load_database(string file_name, int n){
    ifstream fileInput;
    fileInput.open(file_name, ios::in);
    if(fileInput.fail()) return false;
    
    string line;

    for(int i = 0; i < n; i++){
        fileInput >> line;
        
        if(!fileInput.eof()) database_vector.push_back(stoi(line));
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

    fileInput >> database_file_name >> database_amount;
    fileInput >> entries_file_name;

    for(int i = 0; !fileInput.eof(); i++){
        int buf;
        fileInput >> buf;
        instancesSize.push_back(buf);
    };
    fileInput.close();
    return true;
}

void save_entries(string entries_file_name){
    fstream fileOutput;
    fileOutput.open(entries_file_name, ios::out);
    fileOutput << "type of structure,type of operation,size of structure,repetitions of operation,elapsed time" << endl;
    for(int i = 0; i < entries.size(); i++) fileOutput << entries[i] << endl;
    fileOutput.close();
}

//ARRAYS

void arr_create(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);
    
    for(int r = 0; r < repetitions; r++){

        t_1 = Clock::now();

        int *arr = new int[size];               // Utworz wskaznika na tablice i alokuj (size * sizeof(int)) pamieci
        for(int i = 0; i < size; i++) {         // Dla kazdego elementu tablicy (0; size - 1): 
            arr[i] = database_vector[i];        //      Wstaw element z wczytanej bazy danych
            }  
        delete[] arr;

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry arr_create_entry = Entry("array", "create", size, repetitions, t_elapsed.count());
    entries.push_back(arr_create_entry.toString());
}

void arr_put(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);
    
    int *arr = new int[size];
    for(int i = 0; i < size; i++) arr[i] = database_vector[i];

    srand(time(NULL));
    int val = rand() % 9999999;
    int index = rand() % size;

    for(int r = 0; r < repetitions; r++) {
        t_1 = Clock::now();
        
        arr[index] = val;                       // Wstaw konkretna wartosc do konkretnego klucza

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry arr_put_entry = Entry("array", "put", size, repetitions, t_elapsed.count());
    entries.push_back(arr_put_entry.toString());
    
    delete[] arr;
}

void arr_add(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);
    
    for(int r = 0; r < repetitions; r++){
        int *arr = new int[size];
        for(int i = 0; i < size; i++) arr[i] = database_vector[i];
        
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int val = randomNumber(rng);
        
        t_1 = Clock::now();
        
        size_t temp_size = size + 1;                // Ustaw rozmiar tymczasowej tablicy = size + 1
        int *temp_arr = new int[temp_size];         // Utworz wskaznik na nowa tablice i alokuj ((size + 1) * sizeof(int)) pamieci
        memcpy(temp_arr, arr, size * sizeof(int));  // Kopiuj zawartosc tablicy do tymczasowej tablicy
        temp_arr[size] = val;                       // Wstaw do ostatniego elementu tymczasowej tablicy konkretna wartosc
        delete[] arr;                               // Usun stara tablice, dealokuj pamiec
        arr = temp_arr;                             // Ustaw wskaznik tablicy na tymczasowa tablice
        
        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
        
        delete[] temp_arr;
    }
    Entry arr_add_entry = Entry("array", "add", size, repetitions, t_elapsed.count());
    entries.push_back(arr_add_entry.toString());
}


void arr_find(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    int *arr = new int[size];
    for(int i = 0; i < size; i++) arr[i] = database_vector[i];

    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
    int index = randomIndex(rng);
    int val = database_vector[index];

    for(int r = 0; r < repetitions; r++){

        index = rand() % size;
        val = database_vector[index];

        t_1 = Clock::now();

        for(int i = 0; i < size; i++){          // Dla kazdego elementu tablicy (0; size - 1):
            if(arr[i] == val) {                 //      Jezeli element tablicy jest rowny szukanej wartosci:
                t_2 = Clock::now();
                break;                          //          Zakoncz
            }                                   //      W innym wypadku: i++
        }
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry arr_find_entry = Entry("array", "find", size, repetitions, t_elapsed.count());
    entries.push_back(arr_find_entry.toString());
    delete[] arr;
}

void arr_delete(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        int *arr = new int[size];
        for(int i = 0; i < size; i++) arr[i] = database_vector[i];

        t_1 = Clock::now();

        size_t newsize = size - 1;                      // Ustaw rozmiar nowej tablicy = size - 1
        int *temp_arr = new int[newsize];               // Utworz wskaznik na nowa tablice i alokuj ((size + -) * sizeof(int)) pamieci
        memcpy(temp_arr, arr, newsize * sizeof(int));   // Kopiuj zawartosc tablicy do nowej tablicy
        delete[] arr;                                   // Usun stara tablice, dealokuj pamiec
        arr = temp_arr;                                 // Ustaw wskaznik tablicy na nowa tablice

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
        delete[] temp_arr;
    }
    Entry arr_delete_entry = Entry("array", "delete", size, repetitions, t_elapsed.count());
    entries.push_back(arr_delete_entry.toString());
}

//LISTS

class List{
    private:
    typedef struct Node{
        int value;
        Node *next;
    } * node_ptr;

    node_ptr head;

    size_t sz;

    void increment(){ this -> sz++; }
    void decrement(){ this -> sz--; }

    public:
    List(){
        this -> head = nullptr;             // Utworz pusty wskaznik
        this -> sz = 0;                     // Ustaw rozmiar na 0
    }

    ~List(){ this -> emptyList(); }
    
    void add(int value){                            // Tworzy nowy wezel z konkretna wartoscia
        node_ptr newPointer = new Node;             // Utworz nowy wezel
        newPointer -> value = value;                // Ustaw konkretna wartosc wezla
        newPointer -> next = nullptr;               // Ustaw pusty wskaznik wezla 

        if(isEmpty()) this -> head = newPointer;            // Jezeli lista jest pusta: ustaw nowy wezel jako glowe
        else{                                               // W innym wypadku:
            node_ptr previousHeadPointer = this -> head;    //      Zachowaj stara glowe
            this -> head = newPointer;                      //      Ustaw nowy wezel jako nowa glowe
            newPointer -> next = previousHeadPointer;       //      Ustaw wskaznik nowego wezla na stara glowe
        }
        increment();                                        // Zwieksz rozmiar listy
    }

    void remove(int value){
        node_ptr previousPointer = nullptr;                 // Utworz pusty wskaznik na poprzedni wezel
        node_ptr currentPointer = head;                     // Utworz wskaznik na glowe

        if(isEmpty()) return;                               // Jezeli lista pusta: zakoncz
        while(currentPointer != nullptr){                   // Dla kazdego elementu listy (dopoki wskaznik wezla wskazuje na element):
            if(value == currentPointer -> value){           //      Jezeli biezacy wezel ma byc usuniety:
                node_ptr nextNode = currentPointer -> next; //          Utworz pomocniczy wezel - nastepny wezel
                
                if (previousPointer == nullptr) {           //          Jezeli poprzedni wezel jest pusty:
                    this -> head = nextNode;                //              Ustaw nastepny wezel jako głowę
                }
                else previousPointer -> next = nextNode;    //          W przeciwnym wypadku: ustaw wskaznik poprzedniego wezla na kolejny wezel
                
                delete currentPointer;                      //          Dealokuj pamiec
                
                decrement();                                //          Zmniejsz rozmiar listy
                return;
            }
            previousPointer = currentPointer;               // Ustaw wskaznik poprzedniego wezla na aktualny wezel
            currentPointer = currentPointer -> next;        // Ustaw wskaznik aktualnego wezla na nastepny wezel
        }
        return;
    }

    bool find(int value){
        node_ptr currentPointer = this -> head;                 // Utworz pomocniczy wezel - glowa
        while(currentPointer != nullptr){                       // Dla kazdego elementu listy (dopoki wskaznik wezla wskazuje na element): 
            if((currentPointer -> value) == value) return true; //      Jezeli biezacy wezel zawiera szukana wartosc: zakoncz 
            currentPointer = currentPointer -> next;            //      W przeciwnym wypadku: inkrementuj wskaznik
        }
        return false;
    }

    bool isEmpty(){ return this-> head == nullptr; } 

    void emptyList(){
        node_ptr currentPointer;
        while(this -> head != nullptr){
            currentPointer = this -> head;
            this -> head = this -> head -> next;
            delete currentPointer;
        }
        this -> sz = 0;
    }

    size_t size(){ return this -> sz; }

};

void list_create(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){

        t_1 = Clock::now();

        List list;                        // Utworz nowa liste                     
        for(int i = 0; i < size; i++) {   // Dla kazdego elementu listy (0; size - 1): 
            list.add(database_vector[i]); //      Wstaw element z wczytanej bazy danych
        }

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }

    Entry list_create_entry = Entry("list", "create", size, repetitions, t_elapsed.count());
    entries.push_back(list_create_entry.toString());
}

void list_add(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        List list;
        for(int i = 0; i < size; i++) list.add(database_vector[i]);

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int val = randomNumber(rng);

        t_1 = Clock::now();

        list.add(val);                  // Dodaj konkretny element do listy
        
        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry list_add_entry = Entry("list", "add", size, repetitions, t_elapsed.count());
    entries.push_back(list_add_entry.toString());
}

void list_find(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        List list;
        for(int i = 0; i < size; i++) list.add(database_vector[i]);

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
        int index = randomIndex(rng);
        int val = database_vector[index];

        t_1 = Clock::now();

        list.find(val);                 // Wyszukaj konkretny element w liscie

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry list_find_entry = Entry("list", "find", size, repetitions, t_elapsed.count());
    entries.push_back(list_find_entry.toString());
}

void list_delete(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        List list;
        for(int i = 0; i < size; i++) list.add(database_vector[i]);

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
        int index = randomIndex(rng);
        int val = database_vector[index];

        t_1 = Clock::now();

        list.remove(val);           // Usun konkretny element z tablicy
        
        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry list_delete_entry = Entry("list", "delete", size, repetitions, t_elapsed.count());
    entries.push_back(list_delete_entry.toString());
}

//QUEUES

class Queue{
    public:
    int front, rear, size;
    unsigned cap;
    int* arr;
    ~Queue(){ delete[] arr; }
};

Queue* createQueue(unsigned cap){
    Queue* queue = new Queue();             // Utworz nowa kolejke
    queue -> cap = cap;                     // Ustaw pojemnosc
    queue -> front = 0;                     // Ustaw poczatek na 0
    queue -> rear = cap - 1;                // Ustaw koniec na pojemnosc - 1
    queue -> size = 0;                      // Ustaw rozmiar na 0
    queue -> arr = new int[queue -> cap];   // Alokuj (capacity * sizeof(int)) pamieci
    return queue;
}

void deleteQueue(Queue* queue){ delete queue; }

int isFull(Queue* queue) { return (queue -> size == queue -> cap); }

int isEmpty(Queue* queue) { return (queue -> size ==  0); }

void enqueue(Queue* queue, int val){
    if (isFull(queue)) return;                              // Jezeli kolejka jest pelna: zakoncz
    queue -> rear = (queue -> rear + 1) % queue -> cap;     // Ustaw koniec na ((koniec + 1) modulo pojemnosc)
    queue -> arr[queue -> rear] = val;                      // Ustaw konkretny element z tylu kolejki
    queue -> size = queue -> size + 1;                      // Inkrementuj rozmiar kolejki
}

int dequeue(Queue* queue){
    if (isEmpty(queue)) return INT_MIN;                     // Jezeli kolejka jest pusta: zwroc INT_MIN
    int val = queue -> arr[queue -> front];                 // Utworz zmienna i przypisz jej wartosc z poczatku
    queue -> front = (queue -> front + 1) % queue -> cap;   // Ustaw poczatek kolejki na ((poczatek + 1) modulo pojemnosc)
    queue -> size = queue -> size - 1;                      // Dekrementuj rozmiar kolejki
    return val;
}

int front(Queue* queue){
    if (isEmpty(queue)) return INT_MIN;
    return queue -> arr[queue -> front];
}

int rear(Queue* queue){
    if (isEmpty(queue)) return INT_MIN;
    return queue -> arr[queue -> rear];
}

void queue_create(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        t_1 = Clock::now();

        Queue* queue = createQueue(size);       // Utworz nowa kolejke
        for(int i = 0; i < size; i++) {         // Dla kazdego elementu kolejki (0; size - 1):
            enqueue(queue, database_vector[i]); //      Zakolejkuj element z wczytanej bazy danych
        }

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
        delete queue;
    }

    
    Entry queue_create_entry = Entry("queue", "create", size, repetitions, t_elapsed.count());
    entries.push_back(queue_create_entry.toString());
}

void queue_enqueue(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        Queue* queue = createQueue(size);
        for(int i = 0; i < size; i++) enqueue(queue, database_vector[i]);
        
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int val = randomNumber(rng);

        t_1 = Clock::now();

        enqueue(queue, val);            // Zakolejkuj konkretny element

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

        delete queue;
    }
    Entry queue_enqueue_entry = Entry("queue", "enqueue", size, repetitions, t_elapsed.count());
    entries.push_back(queue_enqueue_entry.toString());
}

void queue_find(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        Queue* queue = createQueue(size);
        for(int i = 0; i < size; i++) enqueue(queue, database_vector[i]);
        
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
        int index = randomIndex(rng);
        int val = database_vector[index];

        t_1 = Clock::now();

        Queue* temp_queue = createQueue(size);          // Utworz tymczasowa kolejke o pojemnosci starej kolejki
        int temp_queue_size = 0;                        // Utworz zmienna rozmiaru tymczasowej kolejki
        bool found = false;                             // Utworz zmienna do oznaczania czy operacja sie powiodla
        for(int i = 0; i < size; i++){                  // Dla kazdego elementu starej kolejki (0; size - 1):
            if(front(queue) == val) {                   //      Jezeli element na poczatku kolejki jest rowny szukanej wartosci:
                found = true;                           //          Oznacz, ze operacja sie udala
                break;                                  //          zakoncz
            }
            else enqueue(temp_queue, dequeue(queue));   //      W przeciwnym wypadku: przenies element z poczatku starej kolejki na koniec tymczasowej kolejki
            temp_queue_size++;                          //      Inkrementuj rozmiar tymczasowej kolejki
        }
        for(int i = 0; i < temp_queue_size; i++) {      // Dla kazdego elementu tymczasowej kolejki (0; temp_queue_size - 1):
        enqueue(queue, dequeue(temp_queue));            //      Przenies element z poczatku tymczasowej kolejki na koniec starej kolejki 
        }
        
        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

        delete queue;
    }
    Entry queue_find_entry = Entry("queue", "find", size, repetitions, t_elapsed.count());
    entries.push_back(queue_find_entry.toString());
}

void queue_dequeue(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        Queue* queue = createQueue(size);
        for(int i = 0; i < size; i++) enqueue(queue, database_vector[i]);

        t_1 = Clock::now();

        dequeue(queue);              // Odkolejkuj pierwszy element
        
        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

        delete queue;
    }
    Entry queue_dequeue_entry = Entry("queue", "dequeue", size, repetitions, t_elapsed.count());
    entries.push_back(queue_dequeue_entry.toString());
}

//STACKS

class Stack{
    private:
        int *vals;
        int top;
    public:
        Stack(){
            top = -1;                   // Ustaw wskaznik na -1 (stos jest pusty)
            vals = new int[100000];   // Alokuj (10000000 * sizeof(int)) pamieci
        }

        ~Stack(){ delete[] vals; }

        int push(int n){
            if(isFull()) return 0;  // Jezeli stos jest pelny: zakoncz
            top++;                  // Inkrementuj wskaznik stosu
            vals[top] = n;          // Ustaw element pod wskaznikiem na dodawany element
            return n;               // Zwroc dodawany element
        }

        int pop(){
            int buf;                // Utworz wartosc tymczasowa
            if(isEmpty()) return 0; // Jezeli stos jest pusty: zakoncz
            buf = vals[top];        // Ustaw wartosc tymczasowa na wartosc z wierzcholka
            top--;                  // Dekrementuj wskaznik stosu
            return buf;             // Zwroc wartosc tymczasowa
        }

        int isEmpty(){
            if(top == -1) return 1;
            return 0;
        }

        int isFull(){
            if(top == 9999999) return 1;
            return 0;
        }

        int peek(){
            if(isEmpty()) return 0; // Jezeli stos jest pusty: zakoncz
            return vals[top];       // Zwroc wartosc z wierzcholka stosu
        }
};

void stack_create(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){

        t_1 = Clock::now();

        Stack stack = Stack();              // Utworz nowy stos
        for(int i = 0; i < size; i++) {     // Dla kazdego elementu stosu (0; size - 1):
            stack.push(database_vector[i]); //      Wstaw element z wczytanej bazy danych
        }

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry stack_create_entry = Entry("stack", "create", size, repetitions, t_elapsed.count());
    entries.push_back(stack_create_entry.toString());
}

void stack_push(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++) {
        Stack stack = Stack();
        for(int i = 0; i < size; i++) stack.push(database_vector[i]);

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int val = randomNumber(rng);

        t_1 = Clock::now();

        stack.push(val);            // Dodaj konkretny element na stos

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry stack_push_entry = Entry("stack", "push", size, repetitions, t_elapsed.count());
    entries.push_back(stack_push_entry.toString());
}

void stack_put(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++) {
        Stack stack = Stack();
        for(int i = 0; i < size; i++) stack.push(database_vector[i]);

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int index = randomIndex(rng);
        int val = randomNumber(rng);

        t_1 = Clock::now();

        Stack temp_stack = Stack();         // Utworz tymczasowy stos
        for(int i = 0; i < index; i++) {    // Dla elementow starego stosu przed wymaganym elementem (0; index - 1):
            temp_stack.push(stack.pop());   //      Zdejmij element ze starego stosu i wstaw do stosu tymczasowego
        }
        stack.push(val);                    // Wstaw do starego stosu konkretną wartość
        for(int i = 0; i < index; i++) {    // Dla kazdego elementu tymczasowego stosu (0; index - 1):
            stack.push(temp_stack.pop());   //      Zdejmij element z tymczasowego stosu i wstaw z powrotem do starego stosu
        }

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry stack_put_entry = Entry("stack", "put", size, repetitions, t_elapsed.count());
    entries.push_back(stack_put_entry.toString());
}

void stack_find(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    for(int r = 0; r < repetitions; r++){
        Stack stack = Stack();
        for(int i = 0; i < size; i++) stack.push(database_vector[i]);
        
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> randomIndex(0, size);
        uniform_int_distribution<mt19937::result_type> randomNumber(0, 9999999);
        int index = randomIndex(rng);
        int val = randomNumber(rng);

        t_1 = Clock::now();

        Stack temp_stack = Stack();             // Utworz tymczasowy stos
        int temp_stack_size = 0;                // Utworz zmienna rozmiaru tymczasowego stosu
        bool found = false;

        for(int i = 0; i < size; i++){          // Dla kazdego elementu starego stosu (0; size - 1):
            if(stack.peek() == val){            //      Jezeli element z wierzchu stosu jest rowny szukanej wartoscii
                t_2 = Clock::now();
                found = true;                   //          Oznacz, ze operacja sie udala
                break;                          //          Zakoncz
            }
            else{                               //      W innym wypadku:
                temp_stack.push(stack.pop());   //          Przenies element z wierzchu starego stosu na tymczasowy stos
                temp_stack_size++;              //          Inkrementuj rozmiar tymczasowego stosu
            }
        }
        for(int i = 0; i < temp_stack_size; i++) {  //  Dla kazdego elementu tymczasowego stosu:
            stack.push(temp_stack.pop());           //      Przenies element z wierzchu tymczasowego stosu na nowy stos
        }
        
        if(!found) t_2 = Clock::now();

        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
    }
    Entry stack_find_entry = Entry("stack", "find", size, repetitions, t_elapsed.count());
    entries.push_back(stack_find_entry.toString());
}

void stack_pop(int size, int repetitions){
    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

        for(int r = 0; r < repetitions; r++) {
        Stack stack = Stack();
        for(int i = 0; i < size; i++) stack.push(database_vector[i]);

        t_1 = Clock::now();

        stack.pop();            // Pobierz wartosc ze stosu

        t_2 = Clock::now();
        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
        
    }
    Entry stack_pop_entry = Entry("stack", "pop", size, repetitions, t_elapsed.count());
    entries.push_back(stack_pop_entry.toString());
}

//MAIN

int main(){
    load_config();
    load_database(database_file_name, database_amount);

    int repetitions = 10;
    string entries_file_name_final;

    for(int i = 0; i < instancesSize.size(); i++){

        if(instancesSize[i] < 100) repetitions = 10000;
        if(instancesSize[i] < 1000) repetitions = 1000;
        if(instancesSize[i] < 10000) repetitions = 100; 
        
        cout << "ARRAYS" << endl << "Size: " << instancesSize[i] << ", Repetitions: " << repetitions << endl;
        arr_create(instancesSize[i] , repetitions);
        arr_add(instancesSize[i], repetitions);
        arr_put(instancesSize[i], repetitions);
        arr_find(instancesSize[i], repetitions);
        arr_delete(instancesSize[i], repetitions);   
    }

    entries_file_name_final = "arr_" + entries_file_name;
    save_entries(entries_file_name_final);
    entries.clear();
    cout << endl << "SAVED TO FILE: " << entries_file_name_final << endl;

    for(int i = 0; i < instancesSize.size(); i++){

        if(instancesSize[i] < 100) repetitions = 10000;
        if(instancesSize[i] < 1000) repetitions = 1000;
        if(instancesSize[i] < 10000) repetitions = 100; 
        
        cout << "LIST" << endl << "Size: " << instancesSize[i] << ", Repetitions: " << repetitions << endl;
        list_create(instancesSize[i], repetitions);
        list_add(instancesSize[i], repetitions);
        list_find(instancesSize[i], repetitions);
        list_delete(instancesSize[i], repetitions);
    }

    entries_file_name_final = "list_" + entries_file_name;
    save_entries(entries_file_name_final);
    entries.clear();
    cout << endl << "SAVED TO FILE: " << entries_file_name_final << endl;

    for(int i = 0; i < instancesSize.size(); i++){

        if(instancesSize[i] < 100) repetitions = 1000;
        if(instancesSize[i] < 1000) repetitions = 100;
        if(instancesSize[i] < 10000) repetitions = 10; 
        
        cout << "QUEUE" << endl << "Size: " << instancesSize[i] << ", Repetitions: " << repetitions << endl;
        queue_create(instancesSize[i], repetitions);
        queue_enqueue(instancesSize[i], repetitions);
        queue_find(instancesSize[i], repetitions);
        queue_dequeue(instancesSize[i], repetitions);
    }

    entries_file_name_final = "queue_" + entries_file_name;
    save_entries(entries_file_name_final);
    entries.clear();
    cout << endl << "SAVED TO FILE: " << entries_file_name_final << endl;

    for(int i = 0; i < instancesSize.size(); i++){

        if(instancesSize[i] < 10) repetitions = 1000;
        if(instancesSize[i] < 100) repetitions = 100;
        if(instancesSize[i] < 1000) repetitions = 10; 
        
        cout << "STACK" << endl << "Size: " << instancesSize[i] << ", Repetitions: " << repetitions << endl;
        stack_create(instancesSize[i], repetitions);
        stack_push(instancesSize[i], repetitions);     
        stack_put(instancesSize[i], repetitions);
        stack_find(instancesSize[i], repetitions);
        stack_pop(instancesSize[i], repetitions);
    }

    entries_file_name_final = "stack_" + entries_file_name;
    save_entries(entries_file_name_final);
    entries.clear();
    cout << endl << "SAVED TO FILE: " << entries_file_name_final << endl;
    return 0;
}