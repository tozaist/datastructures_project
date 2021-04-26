#include<iostream>
#include<fstream>
#include<chrono>

using namespace std;

int main(){
    int n;

    auto t_1 = chrono::steady_clock::now();
    auto t_2 = chrono::steady_clock::now();
    chrono::duration<double> t_elapsed = chrono::duration<double>(0);

    fstream fileOutput;
    fileOutput.open("sizes.csv", ios::out);

    cout << "How many integers to generate: ";
    cin >> n;

    cout << "Generating " << n << " worst-case integers..." << endl;


    t_1 = chrono::steady_clock::now();

    for(int i = 0; i <= n; i += 100){
        fileOutput << i << "\n";
    }

    t_2 = chrono::steady_clock::now();
    t_elapsed = chrono::duration_cast<chrono::duration<double>>(t_2 - t_1);

    fileOutput.close();
    cout << "Generated " << n << " integers in " << t_elapsed.count()  << " seconds.";

    return 0;
}