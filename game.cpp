#include <bits/stdc++.h>

using namespace std;



class Game {
private:
    enum Type {
        EMPTY,
        PAWN,
        NIGHT,
        BISHOP,
        ROCK,
        QUEEN,
        KING
    };

    enum Team {
        NONE,
        WHITE,
        BLACK,
    };

    class Piece {
    public:
        Type type;
        Team team;
        Piece(Type a, Team b) : type(a), team(b) {}
    };

    Piece* createPiece(Type type, Team team) {
        return new Piece(type, team);
    }

    class Square {
    public:
        Piece *piece = nullptr;
    };

    Square** board = new Square*[8];

public:
    Game() {
        init();
    }

    void reset() {
        Type list[8] = { ROCK, NIGHT, BISHOP, QUEEN, KING, BISHOP, NIGHT, ROCK };

        for (int i = 0; i < 8; i++) {
            board[0][i].piece = createPiece(list[i], WHITE);
            board[1][i].piece = createPiece(PAWN, WHITE);

            board[7][i].piece = createPiece(list[i], BLACK);
            board[6][i].piece = createPiece(PAWN, BLACK);
        }

        for (int i = 2; i < 6; i++) for (int j = 0; j < 8; j++) {
            board[i][j].piece = nullptr;
        }
    }

    void init() {
        for (int i = 0; i < 8; i++)
            board[i] = new Square[8];

        reset();
    }

    void print(bool isW) {
        if (isW) {
            for (int i = 7; i >= 0; i--) {
                for (int j = 0; j < 8; j++) {
                    board[i][j].piece == nullptr ? 
                        cout << "□ " : cout << board[i][j].piece->type << " ";
                }
                cout << "\n";
            }
            cout << "\n\n";

            for (int i = 7; i >= 0; i--) {
                for (int j = 0; j < 8; j++) {
                    board[i][j].piece == nullptr ?
                        cout << "□ " : cout << board[i][j].piece->team << " ";
                }
                cout << "\n";
            }
        }
        else {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    board[i][j].piece == nullptr ?
                        cout << "□ " : cout << board[i][j].piece->type << " ";
                }
                cout << "\n";
            }
            cout << "\n\n";

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    board[i][j].piece == nullptr ?
                        cout << "□ " : cout << board[i][j].piece->team << " ";
                }
                cout << "\n";
            }
        }
    }
};

int main() {
    Game* chess = new Game();
    chess->print(false);
}