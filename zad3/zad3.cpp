#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;
using Clock = std::chrono::steady_clock;

vector<int> standard_vector;
string standard_filename = "";

int data_amount;

vector<string> entries;
string entries_filename = "";

enum TREE {
    AVL,
    RB
};

string tree_string (TREE tree) {
    switch(tree) {
        case AVL:
            return "AVL";
        case RB:
            return "RB";
        default:
            return "Invalid";
    }
}

enum ACTION {
    CREATE,
    INSERT,
    DELETE,
    SEARCH
};

string action_string (ACTION action) {
    switch(action) {
        case CREATE:
            return "CREATE";
        case INSERT:
            return "INSERT";
        case DELETE:
            return "DELETE";
        case SEARCH:
            return "SEARCH";
        default:
            return "Invalid";
    }
}

struct Operation {
    TREE tree;
    ACTION action;
    int size;
    int reps;

    Operation(string tree, string action, int size, int reps) {
        if (tree.compare("RB") == 0) {
            this->tree = RB;
        } else {
            this->tree = AVL;
        }
        if (action.compare("CREATE") == 0) {
            this->action = CREATE;
        } else {
            if (action.compare("INSERT") == 0) {
                this->action = INSERT;
            } else {
                if (action.compare("DELETE") == 0) {
                    this->action = DELETE;
                } else {
                    this->action = SEARCH;
                }
            }
        }
        this->size = size;
        this->reps = reps;
    }
};

vector<Operation> operations;

struct Entry {
    string tree;
    string action;
    int size;
    int reps;
    double time;
    
    Entry(string tree, string action, int size, int reps, double time){
        this->tree = tree;
        this->action = action;
        this->size = size;
        this->reps = reps;
        this->time = time;
    }

    string toString(){
        return(tree + "," + action + "," + to_string(size) + "," + to_string(reps) + "," + to_string(time));
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
    fileInput.open("config.ini", ios::in);
    if(fileInput.fail()) {
        fileInput.close();
        return false;
    }

    fileInput >> standard_filename;
    fileInput >> data_amount;
    fileInput >> entries_filename;

    for(int i = 0; !fileInput.eof(); i++){
        string tree, action;
        int min, max, step, reps;
        fileInput >> tree;
        fileInput >> action;
        fileInput >> min;
        fileInput >> max;
        fileInput >> step;
        fileInput >> reps;
        while(min <= max) {
            Operation newOp(tree, action, min, reps);
            operations.push_back(newOp);
            min += step;
        }
    };
    fileInput.close();
    return true;
}

void save_entries(string entries_file_name) {
    fstream fileOutput;
    fileOutput.open(entries_file_name, ios::out);
    fileOutput << "tree type,action,size,reps,time" << endl;
    for(int i = 0; i < entries.size(); i++) fileOutput << entries[i] << endl;
    fileOutput.close();
}

// Drzewo AVL
struct AVLNode {
    AVLNode *parent;
    AVLNode *left;
    AVLNode *right;
    int key;
    int bf;
};

void DFSRelease (AVLNode *n) {
    if (n) {
        DFSRelease (n->left);
        DFSRelease (n->right);
        delete n;
    }
}

void rightRight(AVLNode *&root, AVLNode *a) {
    AVLNode *b = a->right;
    AVLNode *parent = a->parent;

    a->right = b->left;
    if (a->right != NULL) {
        a->right->parent = a;
    }

    b->left = a;
    b->parent = parent;
    a->parent = b;

    if (parent != NULL) {
        if (parent->left == a) {
            parent->left = b;
        } else {
            parent->right = b;
        }
    } else {
        root = b;
    }
    if (b->bf == -1) {
        b->bf = 0;
        a->bf = 0;
    } else {
        a->bf = -1;
        b->bf = 1;
    }
}

void leftLeft(AVLNode *&root, AVLNode *a) {
    AVLNode *b = a->left;
    AVLNode *parent = a->parent;

    a->left = b->right;
    if (a->left != NULL) {
        a->left->parent = a;
    }
    b->right = a;
    b->parent = parent;
    a->parent = b;

    if (parent != NULL) {
        if (parent->left = a) {
            parent->left = b;
        } else {
            parent->right = a;
        }
    } else {
        root = b;
    }
    if (b->bf == 1) {
        b->bf = 0;
        a->bf = 0;
    } else {
        a->bf = 1;
        b->bf = -1;
    }
}

void rightLeft(AVLNode *&root, AVLNode *a) {
    AVLNode *b = a->right;
    AVLNode *c = b->left;
    AVLNode *parent = a->parent;

    b->left = c->left;
    if (b->left != NULL) {
        b->left->parent = b;
    }
    a->right = c->left;
    if (a->right != NULL) {
        a->right->parent = a;
    }
    c->left = a;
    c->right = b;
    a->parent = c;
    b->parent = c;
    c->parent = parent;

    if (parent != NULL) {
        if (parent->left == a) {
            parent->left = c;
        } else {
            parent->right = c;
        }
    } else {
        root = c;
    }

    if (c->bf == -1) {
        a->bf = 1;
    } else {
        a->bf = 0;
    }
    if (c->bf == 1) {
        b->bf = -1;
    } else {
        b->bf = 0;
    }

    c->bf = 0;
}

void leftRight(AVLNode *&root, AVLNode *a) {
    AVLNode *b = a->left;
    AVLNode *c = b->right;
    AVLNode *parent = a->parent;

    b->right = c->left;
    if (b->right != NULL) {
        b->right->parent = b;
    }
    a->left = c->right;
    if (a->left != NULL) {
        a->left->parent = a;
    }

    c->right = a;
    c->left = b;
    a->parent = c;
    b->parent = c;
    c->parent = parent;

    if (parent != NULL) {
        if (parent->left == a) {
            parent->left = c;
        } else {
            parent->right = c;
        }
    } else {
        root = c;
    }

    if (c->bf == 1) {
        a->bf = -1;
    } else {
        a->bf = 0;
    }
    if (c->bf == -1) {
        b->bf = 1;
    } else {
        b->bf = 0;
    }

    c->bf = 0;
}

void insertAVL(AVLNode *&root, int key) {
    AVLNode *newNode = new AVLNode;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->key = key;
    newNode->bf = 0;

    AVLNode *parent = root;
    if (parent == NULL) {
        root = newNode;
    } else {
        while(true) {
            if (key < parent->key) {
                if (parent->left == NULL) {
                    parent->left = newNode;
                    break;
                }
                parent = parent->left;
            } else {
                if (parent->right == NULL) {
                    parent->right = newNode;
                    break;
                }
                parent = parent->right;
            }
        }
    }
    newNode->parent = parent;
    parent = root;
    if (parent->bf) {
        parent->bf = 0;
    } else {
        if (parent->left == newNode) {
            parent->bf = 1;
        } else {
            parent->bf = -1;
        }

        AVLNode *grandparent = parent->parent;

        bool t = false;
        while (grandparent != NULL) {
            if (grandparent->bf) {
                grandparent->bf = 1;
            } else {
                grandparent->bf = -1;
            }
            parent = grandparent;
            grandparent = grandparent->parent;
        }

        if (t) {
            if (grandparent->bf == 1) {
                if (grandparent->right == parent) {
                    grandparent->bf = 0;
                } else {
                    if (parent->bf == -1) {
                        leftRight(root, grandparent);
                    } else {
                        leftLeft(root, grandparent);
                    }
                }
            } else {
                if (grandparent->left == parent) {
                    grandparent->bf = 0;
                } else {
                    if (parent->bf == 1) {
                        rightLeft(root, grandparent);
                    } else {
                        rightRight(root, grandparent);
                    }
                }
            }
        }
    }
}

AVLNode *predecessor (AVLNode *n) {
    AVLNode *o;

    if (n != NULL) {
        if (n->left != NULL) {
            n = n->left;
            while (n->right != NULL) {
                n = n->right;
            }
        } else {
            do {
                o = n;
                n = n->parent;
            } while (n != NULL && n->right != o);
        }
    }
    return n;
}

AVLNode *searchAVL(AVLNode *&root, int key) {
    AVLNode *n = root;
    while (n && n->key != key) {
        n = (key < n->key) ? n->left : n->right;
    }
    return n;
}

AVLNode *deleteAVL (AVLNode *&root, AVLNode *n) {
    AVLNode *o, *p, *r;
    bool nested;
    if (n == NULL) {
        return n;
    }
    if (n->left != NULL && n->right != NULL) {
        o = deleteAVL(root, predecessor(n));
        nested = false;
    } else {
        if (n->left != NULL) {
            o = n->left;
            n->left = NULL;
        } else {
            o = n->right;
            n->right = NULL;
        }
        n->bf = 0;
        nested = true;
    }
    if (o != NULL) {
        o->parent = n->parent;
        o->left = n->left;
        o->right = n->right;
        o->bf = n->bf;
        if (o->left != NULL) {
            o->left->parent = o;
        }
        if (o->right != NULL) {
            o->right->parent = o;
        }
    }

    if (n->parent != NULL) {
        if (n->parent->left == n) {
            n->parent->left = o;
        } else {
            n->parent->right = o;
        }
    } else {
        root = o;
    }
    
    if (nested) {
        p = o;
        o = n->parent;
        while (o != NULL) {
            if (!o->bf) {
                if (o->left == p) {
                    o->bf = -1;
                } else {
                    o->bf = 1;
                }
                break;
            } else {
                if (((o->bf == 1) && (o->left == p)) || ((o->bf == -1) && (o->right == p))) {
                    o->bf = 0;
                    r = o;
                    o = o->parent;
                } else {
                    if (o->left == p) {
                        r = o->right;
                    } else {
                        r = o->left;
                    }
                    if (!r->bf) {
                        if (o->bf == 1) {
                            leftLeft(root, o);
                        } else {
                            rightRight(root, o);
                        }
                        break;
                    } else {
                        if (o->bf == r->bf) {
                            if (o->bf == 1) {
                                leftLeft(root, o);
                            } else {
                                rightRight(root, o);
                            }
                            p = r;
                            o = r->parent;
                        }
                        else {
                            if (o->bf == 1) {
                                leftRight(root, o);
                            } else {
                                rightLeft(root, o);
                            }
                            r = o->parent;
                            o = r->parent;
                        }
                    }
                }
            }
        }
    }
    return n;
}

void printAVL(AVLNode *n) {
    AVLNode *temp = n;
    if (temp != NULL) {
        printAVL(temp->left);
        printAVL(temp->right);
    }
}

// Drzewo czerwono-czarne

// Kolor wezla drzewa RB
enum Color {
    RED,
    BLACK
};

struct RBNode {
    RBNode *parent;
    RBNode *left;
    RBNode *right;
    int key;
    Color color;
};

class RBTree {
    private:
    RBNode g;
    RBNode *root;

    public:
    RBTree();
    ~RBTree();
    void DFSRelease (RBNode *n);
    RBNode *searchRB (int k);
    RBNode *minRB (RBNode *n);
    RBNode *succRB (RBNode *n);
    void rotateLeft (RBNode *a);
    void rotateRight (RBNode *a);
    void insertRB (int k);
    void deleteRB (RBNode *n);
};

RBTree::RBTree() {
    g.color = BLACK;
    g.parent = &g;
    g.left = &g;
    g.right = &g;
    root = &g;
}

RBTree::~RBTree() {
    DFSRelease (root);
}

void RBTree::DFSRelease(RBNode *n) {
    if (n != &g) {
        DFSRelease(n->left);
        DFSRelease(n->right);
        delete n;
    }
}

RBNode * RBTree::searchRB (int k) {
    RBNode *n;

    n = root;
    while ((n != &g) && (n->key != k)) {
        if (k < n->key) {
            n = n->left;
        } else {
            n = n->right;
        }
    }
    if (n == &g) {
        return NULL;
    }
    return n;
}

RBNode * RBTree::minRB (RBNode *n) {
    if (n != &g) {
        while (n->left != &g) {
            n = n->left;
        }
    }
    return n;
}

RBNode * RBTree::succRB (RBNode *n) {
    RBNode *o;

    if (n != &g) {
        if (n->right != &g) {
            return minRB (n->right);
        } else {
            o = n->parent;
            while ((o != &g) && (n == o->right)) {
                n = o;
                o = o->parent;
            }
            return o;
        }
    }
    return &g;
}

void RBTree::rotateLeft (RBNode *a) {
    RBNode *b = a->right;
    RBNode *n;

    if (b != &g) {
        n = a->parent;
        a->right = b->left;
        if (a->right != &g) {
            a->right->parent = a;
        }

        b->left = a;
        b->parent = n;
        a->parent = b;

        if (n != &g) {
            if (n->left == a) {
                n->left = b;
            } else {
                n->right = b;
            }
        } else {
            root = b;
        }
    }
}

void RBTree::rotateRight (RBNode *a) {
    RBNode *b = a->left;
    RBNode *n;

    if (b != &g) {
        n = a->parent;
        a->left = b->right;
        if (a->left != &g) {
            a->left->parent = a;
        }

        b->right = a;
        b->parent = n;
        a->parent = b;

        if (n != &g) {
            if (n->left == a) {
                n->left = b;
            } else {
                n->right = b;
            }
        } else {
            root = b;
        }
    }
}

void RBTree::insertRB (int k) {
    RBNode *a = new RBNode;
    a->parent = root;
    a->left = &g;
    a->right = &g;
    a->key = k;

    RBNode *b;
    if (a->parent == &g) {
        root = a;
    } else {
        while (true) {
            if (k < a->parent->key) {
                if (a->parent->left == &g) {
                    a->parent->left = a;
                    break;
                }
                a->parent = a->parent->left;
            } else {
                if (a->parent->right == &g) {
                    a->parent->right = a;
                    break;
                }
                a->parent = a->parent->right;
            }
        }
        a->color = RED;
        while ((a != root) && (a->parent->color == RED)) {
            if( a->parent == a->parent->parent->left) {
                b = a->parent->parent->right;

                if (b->color == RED) {
                    a->parent->color = BLACK;
                    b->color = BLACK;
                    a->parent->parent->color = RED;
                    a = a->parent->parent;
                    continue;
                }

                if (a == a->parent->right) {
                    a = a->parent;
                    rotateLeft(a);
                }

                a->parent->color = BLACK;
                a->parent->parent->color = RED;
                rotateRight (a->parent->parent);
                break;
            } else {
                b = a->parent->parent->left;

                if (b->color == RED) {
                    a->parent->color = BLACK;
                    b->color = BLACK;
                    a->parent->parent->color = RED;
                    a = a->parent->parent;
                    continue;
                }

                if (a == a->parent->left) {
                    a = a->parent;
                    rotateRight(a);
                }

                a->parent->color = BLACK;
                a->parent->parent->color = RED;
                rotateLeft(a->parent->parent);
                break;
            }
        }
        root->color = BLACK;
    }
}

void RBTree::deleteRB (RBNode *a) {
    RBNode *b;
    RBNode *c;
    RBNode *n;

    if ((a->left == &g) || (a->right == &g)) {
        b = a;
    } else {
        b = succRB (a);
    }

    if (b->left != &g) {
        c = b->left;
    } else {
        c = b->right;
    }

    c->parent = b->parent;

    if (b->parent == &g) {
        root = c;
    } else {
        if (b == b->parent->left) {
            b->parent->left = c;
        } else {
            b->parent->right = c;
        }
    }

    if (b != a) {
        a->key = b->key;
    }

    if (b->color == BLACK) {
        while ((c != root) && (c->color == BLACK)) {
            if (c == c->parent->left) {
                n = c->parent->right;
                if (n->color = RED) {
                    n->color = BLACK;
                    c->parent->color = RED;
                    rotateLeft(c->parent);
                    n = c->parent->right;
                }
                if ((n->left->color == BLACK) && (n->right->color == BLACK)) {
                    n->color = RED;
                    c = c->parent;
                    continue;
                }

                if (n->right->color == BLACK) {
                    n->left->color = BLACK;
                    n->color = RED;
                    rotateRight(n);
                    n = c->parent->right;
                }

                n->color = c->parent->color;
                c->parent->color = BLACK;
                n->right->color = BLACK;
                rotateRight(c->parent);
                c = root;
            } else {
                n = c->parent->left;

                if (n->color == RED) {
                    n->color = BLACK;
                    c->parent->color = RED;
                    rotateRight(c->parent);
                    n = c->parent->left;
                }

                if ((n->left->color == BLACK) && (n->right->color == BLACK)) {
                    n->color = RED;
                    c = c->parent;
                    continue;
                }

                if (n->left->color == BLACK) {
                    n->right->color = BLACK;
                    n->color = RED;
                    rotateLeft(n);
                    n = c->parent->left;
                }

                n->color = c->parent->color;
                c->parent->color = BLACK;
                n->left->color = BLACK;
                rotateRight (c-> parent);
                c = root;
            }
        }
    }
    c->color = BLACK;
    delete b;
}

int main() {
    load_config();
    load_data(standard_filename, data_amount);

    auto t_1 = Clock::now();
    auto t_2 = Clock::now();
    chrono::duration<double> t_elapsed;

    int i = 0;
    int j = 0;

    time_t t;
    mt19937 rng(time(&t));
    int val;
    
    for(Operation operation : operations) {
        
        uniform_int_distribution<int> randomIndex(operation.reps, operation.size);
        t_elapsed = chrono::duration<double>(0);
        
        if (operation.tree == RB) {
            RBTree *tree = new RBTree;
            for(i = 0; i < operation.size; i++) {
                tree->insertRB (standard_vector[i]);
            }
            switch (operation.action) {
                case CREATE:
                    for(i = 0; i < operation.reps; i++) {
                        t_1 = Clock::now();
                        for(j = 0; j < operation.size; j++) {
                            tree->insertRB (standard_vector[j]);
                        }
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;
                case INSERT:
                    for(i = 0; i < operation.reps; i++) {
                        val = randomIndex(rng);
                        t_1 = Clock::now();
                        tree->insertRB(standard_vector[val]);
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;
                case DELETE:
                    val = randomIndex(rng);
                    for(i = 0; i < operation.reps; i++) {
                        t_1 = Clock::now();
                        tree->deleteRB(tree->searchRB(standard_vector[val]));
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                        val--;
                    }
                    break;
                case SEARCH:
                    for(i = 0; i < operation.reps; i++) {
                        val = randomIndex(rng);
                        t_1 = Clock::now();
                        tree->searchRB(standard_vector[val]);
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;
            }
            delete tree;
        } else {
            AVLNode *root = NULL;
            for(i = 0; i < operation.size; i++) {
                insertAVL (root, standard_vector[i]);
            }
            switch (operation.action) {
                case CREATE:
                    for(i = 0; i < operation.reps; i++) {
                        t_1 = Clock::now();
                        for(j = 0; j < operation.size; j++) {
                            insertAVL (root, standard_vector[j]);
                        }
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;

                case INSERT:
                    for(i = 0; i < operation.reps; i++) {
                        val = randomIndex(rng);
                        t_1 = Clock::now();
                        insertAVL (root, standard_vector[val]);
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;

                case DELETE:
                    val = randomIndex(rng);
                    for(i = 0; i < operation.reps; i++) {
                        t_1 = Clock::now();
                        deleteAVL (root, searchAVL(root, standard_vector[val]));
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                        val--;
                    }
                    break;

                case SEARCH:
                    for(i = 0; i < operation.reps; i++) {
                        val = randomIndex(rng);
                        t_1 = Clock::now();
                        searchAVL(root, standard_vector[val]);
                        t_2 = Clock::now();
                        t_elapsed += chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);
                    }
                    break;
            }
            DFSRelease(root);
        }

        cout << "Finished " + action_string(operation.action) + 
        " for " + tree_string(operation.tree) + 
        ", size: " + to_string(operation.size) + 
        ", reps: " + to_string(operation.reps) +
        ", total time: " + to_string(t_elapsed.count()) + "s\n"; 
        Entry entry = Entry(tree_string(operation.tree), action_string(operation.action), operation.size, operation.reps, t_elapsed.count());
        entries.push_back(entry.toString());

    }

    save_entries(entries_filename);
    entries.clear();

    return 0;
}