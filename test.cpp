#include <iostream>
#include <random>
#include <windows.h>

using namespace std;

int moveList[4][4] = { // scotch gambit Bc5 variation
    {4, 6, 4, 4},
    {1, 7, 2, 5},
    {4, 4, 3, 3},
    {5, 7, 2, 4}
};

int main(){
    random_device rd; mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 3);

    int idx(0);
    while (true) {
        int rand = dis(gen);
        int prevMove[4]; cin >> prevMove[0] >> prevMove[1] >> prevMove[2] >> prevMove[3];
        Sleep(rand * 700);
        cout << moveList[idx][0] << " " << moveList[idx][1] << " " << moveList[idx][2] << " " << moveList[idx][3] << endl;
        idx++;
    }
    
}