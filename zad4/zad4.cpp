#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <chrono>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;
using Clock=std::chrono::steady_clock;

vector<int> standard_vector;
string standard_filename="";

int data_amount;

vector<string> entries;
string entries_filename="";

enum GRAPH
{
    ADJACENCY_MATRIX,
    ADJACENCY_LIST
};

string tree_string (GRAPH graph)
{
    switch(graph)
    {
        case ADJACENCY_MATRIX:
            return "ADJACENCY_MATRIX";
        case ADJACENCY_LIST:
            return "ADJACENCY_LIST";
        default:
            return "Invalid";
    }
}

enum ACTION
{
    MST,
    PATH,
};

string action_string (ACTION action)
{
    switch(action)
    {
        case MST:
            return "MST";
        case PATH:
            return "PATH";
        default:
            return "Invalid";
    }
}

struct Operation
{
    GRAPH   graph;
    ACTION  action;
    int     size;
    float   density;
    int     reps;

    Operation(string graph, string action, int size, float density, int reps)
    {
        if (graph.compare("ADJACENCY_MATRIX") == 0)
        {
            this->graph=ADJACENCY_MATRIX;
        } else
        {
            this->graph=ADJACENCY_LIST;
        }
        if (action.compare("MST") == 0)
        {
            this->action=MST;
        } else
        {
            this->action=PATH;
        }
        this->density=density;
        this->size=size;
        this->reps=reps;
    }
};

vector<Operation> operations;

struct Entry
{
    string  graph;
    string  action;
    int     size;
    float   density;
    int     reps;
    double  time;
    
    Entry(string graph, string action, int size, float density, int reps, double time)
    {
        this->graph=graph;
        this->action=action;
        this->size=size;
        this->density=density;
        this->reps=reps;
        this->time=time;
    }

    string toString()
    {
        return(graph + "," + action + "," + to_string(size) + "," + to_string(density) + "," + to_string(reps) + "," + to_string(time));
    }
};

bool load_data(string standard_filename, int n)
{
    ifstream fileInput;

    fileInput.open(standard_filename, ios::in);
    if(fileInput.fail()) return false;
    
    string line;

    for(int i=0; i<n; i++)
    {
        fileInput >> line;
        if(!fileInput.eof()) standard_vector.push_back(stoi(line));
        else return false;
    }
    fileInput.close();
    return true;
}

bool load_config()
{
    ifstream fileInput;
    fileInput.open("config.ini", ios::in);
    if(fileInput.fail()) {
        fileInput.close();
        return false;
    }

    fileInput >> standard_filename;
    fileInput >> data_amount;
    fileInput >> entries_filename;

    for(int i=0; !fileInput.eof(); i++)
    {
        string graph, action;
        int min, max, step, reps;
        float density;
        fileInput >> graph;
        fileInput >> action;
        fileInput >> min;
        fileInput >> max;
        fileInput >> step;
        fileInput >> density;
        fileInput >> reps;
        while(min<=max) {
            Operation newOp(graph, action, min, density, reps);
            operations.push_back(newOp);
            min+=step;
        }
    };
    fileInput.close();
    return true;
}

void save_entries(string entries_file_name)
{
    fstream fileOutput;
    fileOutput.open(entries_file_name, ios::out);
    fileOutput << "graph representation,action,size,reps,time" << endl;
    for(int i=0; i<entries.size(); i++) fileOutput << entries[i] << endl;
    fileOutput.close();
}

struct  AdjMatrixNode
{
    int n;
    int val;

    AdjMatrixNode(int n, int val)
    {
        this->n = n;
        this->val = val;
    }

    AdjMatrixNode()
    {
        n, val=0;
    }
};

// Macierz sasiedztwa
class AdjacencyMatrix
{
    private:
    AdjMatrixNode   *vertices;
    bool            **edges;
    int             number_vertices;

    public:
    AdjacencyMatrix(int nVertices, double density)
    {
        vertices=new AdjMatrixNode[nVertices];
        for(int i=0; i<nVertices; i++)
        {
            vertices[i].n=i;
            vertices[i].val=standard_vector[i];
        }

        edges=new bool*[nVertices];
        for(int i=0; i<nVertices; i++)
        {
            edges[i] = new bool[nVertices];
            memset(edges[i], 0, nVertices*sizeof(bool));
        }

        this->number_vertices=nVertices;
        this->fillEdges(density);
    }

    void fillEdges(double density)
    {
        // D = (2|E|) / (|V|(|V|-1)) -> |E| = (D(|V|(|V|-1)))/2
        int nEdges = (density*(this->number_vertices*(this->number_vertices-1)))/2;

        time_t t;
        mt19937 rng(time(&t));
        uniform_int_distribution<int> distr1(0, this->number_vertices-1);
        uniform_int_distribution<int> distr2(0, this->number_vertices-2);        
        int i,j;

        while (nEdges > 0)
        {
            i=distr1(rng);

            uniform_int_distribution<int> distr3(0, i-1);
            uniform_int_distribution<int> distr4(i+1, this->number_vertices-1);

            j=distr2(rng);
            if (j<i)
                j=distr3(rng);
            else
                j=distr4(rng);

            if(edges[i][j] != true)
            {
                edges[i][j] = true;
                edges[j][i] = true;
                nEdges--;
            }
        }
    }

    void insertVertex(int val)
    {
        this->number_vertices=this->number_vertices+1;

        AdjMatrixNode *newVertices = new AdjMatrixNode[this->number_vertices];
        memcpy(newVertices, this->vertices, (this->number_vertices-1)*sizeof(AdjMatrixNode));
        newVertices[this->number_vertices-1].n=newVertices[this->number_vertices-2].n + 1;
        newVertices[this->number_vertices-1].val = val;
        
        delete[] this->vertices;
        this->vertices = newVertices;

        bool **newEdges = new bool*[this->number_vertices];
        memcpy(newEdges, this->edges, (this->number_vertices-1)*sizeof(bool));
        for(int i=0; i<this->number_vertices-1; i++)
        {
            newEdges[i] = new bool[this->number_vertices];
            memcpy(newEdges[i], edges[i], (this->number_vertices-1)*sizeof(bool));
            newEdges[i][this->number_vertices-1] = 0;
        }
        memset(newEdges[this->number_vertices-1], 0, (this->number_vertices)*sizeof(bool));

        delete[] this->edges;
        this->edges = newEdges;
    }

    void removeVertex(int n)
    {
        int i,j;

        this->number_vertices=this->number_vertices-1;

        AdjMatrixNode *newVertices = new AdjMatrixNode[this->number_vertices];
        memcpy(newVertices, this->vertices, n*sizeof(AdjMatrixNode));
        memcpy(newVertices + n, vertices + n+1, (this->number_vertices-n)*sizeof(AdjMatrixNode));

        for(i=0; i<this->number_vertices; i++)
            newVertices[i].n = i;

        delete[] this->vertices;
        this->vertices = newVertices;

        bool **newEdges = new bool*[this->number_vertices];
        memcpy(newEdges, this->edges, n*sizeof(bool));
        memcpy(newEdges + n, edges + n+1, (this->number_vertices-n)*sizeof(bool));

        for(i=j=0; i<this->number_vertices+1; i++)
        {
            if (i == n)
                continue;
            newEdges[j] = new bool[this->number_vertices];
            memcpy(newEdges[j], this->edges[i], n*sizeof(bool));
            memcpy(newEdges[j] + n, edges[i] + n+1, (this->number_vertices-n)*sizeof(bool));
            j++;
        }

        delete[] this->edges;
        this->edges = newEdges;
    }

    bool insertEdge(int u, int v)
    {
        if (this->edges[u][v] == 1)
            return false;
        this->edges[u][v]=1;
        this->edges[v][u]=1;
        return true;
    }

    bool removeEdge(int u, int v)
    {
        if (this->edges[u][v] == 0)
            return false;
        this->edges[u][v]=0;
        this->edges[v][u]=0;
        return true;
    }

    void print()
    {
        for(int i=0; i<this->number_vertices; i++)
            cout << vertices[i].n << " | " << vertices[i].val << endl;
        for(int i=0; i<this->number_vertices; i++)
        {
            for(int j=0; j<this->number_vertices; j++)
                cout << edges[i][j] << " ";
            cout << endl;
        }
    }
};
// Lista sasiedztwa
class AdjacencyList
{

};

int main()
{
    load_data("standard.csv", 1000);
    int nVertices = 3;
    AdjacencyMatrix *matrix = new AdjacencyMatrix(nVertices, 0);
    matrix->print();
    matrix->insertEdge(1,2);
    cout << "========\n";
    matrix->print();
    matrix->removeEdge(1,2);
    cout << "========\n";
    matrix->print();
}