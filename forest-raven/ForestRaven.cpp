#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "utility.h"
#include "chess.h"

class State {
private:
    map<Type, int> piece_value;
    const Chess* chess = nullptr;
    Color color = NOCOLOR;
    int control_square = 0, threat = 0, defend = 0, hanging = 0, score = 0;

    void update_score() {
        score = 0;
    }

public:
    State(const Chess* ch, Color co){
        chess = ch;
        color = co;
        piece_value[PAWN] = 1;
        piece_value[BISHOP] = 3;
        piece_value[KNIGHT] = 3;
        piece_value[ROOK] = 5;
        piece_value[QUEEN] = 9;
    }

    void update() {
        control_square = 0, threat = 0, defend = 0, hanging = 0;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                int control = chess->board[y][x].attack_wb[color];
                bool isEmpty = chess->board[y][x].empty();
                Color piece_color = chess->board[y][x].piece.color;

                control_square += control;
                if (control != 0 && !isEmpty) // piece in control square
                    piece_color == color ? defend++ : threat++;
                if (control == 0 && !isEmpty && piece_color == color) // Hanging piece
                    hanging++;
            }
    }
};

class Algorithm {
private:
    const int Depth = 1;
    Chess chess;

    Move findBestMove() {
        set<Move>* moveList = chess.get_candidateMove(chess.myColor);
        for (int depth = 1; depth <= Depth; depth++) {
            vector<Chess> vec;
            set<Move>* moveList = chess.get_candidateMove(chess.myColor);
            for (const Move move : *moveList) {
                Chess next = chess.clone();
                next.move(move); // TODO -> Promotion case 
                vec.push_back(next);
            }
        }
        int idx = get_random(0, moveList->size() - 1);
        auto it = moveList->begin();
        advance(it, idx);
        return *it;
    }

public:
    Algorithm(const Chess ch) { chess = ch; }
    Move getBestMove() { return findBestMove(); }
};

class ForestRaven {
private:
    Chess chess;

    Move find_nextMove() {
        Algorithm algo(chess);
        Move move = algo.getBestMove();
        return move;
    }

public:
    int score_wb[2];

    ForestRaven() { init(); }

    void init() {
        int color_int; cin >> color_int;
        string temp; getline(cin, temp);
        chess.myColor = (color_int == 0 ? WHITE : BLACK);
        score_wb[0] = score_wb[1] = 0;
    }

    void start() {
        //board.print_board();
        while (true) {
            chess.turn != chess.myColor ? opponent_move() : move();
            //board.print_board();
        }
    }

    void opponent_move() {
        string line; getline(cin, line); stringstream ss(line); string word;
        auto get_int = [&]() -> int { getline(ss, word, ' '); return stoi(word);; };
        Position cur = Position(get_int(), get_int());
        Position dest = Position(get_int(), get_int());
        int promotion = get_int();
        chess.move(Move(cur, dest, promotion));
    }
    void move() {
        Move move = find_nextMove();
        chess.move(move);
        send_move(move);
    }
    void send_move(const Move& move) { 
        cout << move.ori.x << ' ' << move.ori.y << ' ' << move.dest.x << ' ' << move.dest.y << ' ' << move.promotion << "\n";
    }
};

int main() {
    ForestRaven forest_raven;
    forest_raven.start();
}