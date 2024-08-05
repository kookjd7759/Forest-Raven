#include <iostream>
#include <random>
#include <windows.h>

using namespace std;

int tempIdx = 0;

int moveList[4][4] = { // scotch gambit Bc5 variation
    {4, 6, 4, 4},
    {1, 7, 2, 5},
    {4, 4, 3, 3},
    {5, 7, 2, 4}
};

void restart(){
    tempIdx = 0;
}

bool input(int* move){
    cin >> move[0] >> move[1] >> move[2] >> move[3];
    if (move[0] + move[1] + move[2] + move[3] == -4){ // restart signal
        restart();
        return false;
    }

    return true;
}

int main(){
    random_device rd; mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 3);

    while (true) {
        int move[4]; 
        if (!input(move)) continue; // restart
        Sleep(dis(gen) * 400);
        cout << moveList[tempIdx][0] << " " << moveList[tempIdx][1] << " " << moveList[tempIdx][2] << " " << moveList[tempIdx][3] << endl;
        tempIdx++;
    }
}