#include<iostream>
#include<fstream>
#include<random>
#include<chrono>

using namespace std;

int main(){
    int n;
    int a = -9999999;
    int b = 9999999;

    auto t_1 = chrono::steady_clock::now();
    auto t_2 = chrono::steady_clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    fstream fileOutput;
    fileOutput.open("standard.csv", ios::out);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(a, b);

    cout << "How many integers to generate: ";
    cin >> n;

    cout << "Generating " << n << " integers in range (" << a << "; " << b << ")..." << endl;


    t_1 = chrono::steady_clock::now();

    for(int i = 0; i < n; i++){
        int buf = distr(gen);
        fileOutput << buf << "\n";
    }

    t_2 = chrono::steady_clock::now();
    t_elapsed = chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

    fileOutput.close();
    cout << "Generated " << n << " integers in " << t_elapsed.count()  << " seconds.";

    return 0;
}