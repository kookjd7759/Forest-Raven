#include "engine.h"

using namespace ForestRaven;

int main(void) {
    int c; cin >> c; string t; getline(cin, t);
    Engine engine((Color)c);
    engine.play();
    //Engine engine(BLACK);
    //engine.play_test();
}