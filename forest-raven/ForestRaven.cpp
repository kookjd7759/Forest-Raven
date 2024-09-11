#include <string>
#include <map>
#include <vector>
#include "utility.h"
#include "chess.h"

class State {
private:
    map<Piece_type, int> piece_value;
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
        cout << "findBestMove()\n";
        set<Move>* moveList = chess.get_candidateMove(chess.myColor);
        for (int depth = 1; depth <= Depth; depth++) {
            vector<Chess> vec;
            set<Move>* moveList = chess.get_candidateMove(chess.myColor);
            for (const Move& move : *moveList) {
                Chess next = chess.clone();
                next.play(move);
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
            chess.turn != chess.myColor ? opponent_play() : play();
            //board.print_board();
        }
    }

    void opponent_play() {
        cout << "opponent_play()\n";
        string line; getline(cin, line);
        Move move; move.string_init(line);
        Piece piece = chess.board[move.ori.y][move.ori.x].piece;
        move.piece = piece;
        chess.play(move);
    }
    void play() {
        cout << "play()\n";
        Move move = find_nextMove();
        chess.play(move);
        send_play(move);
    }
    void send_play(Move& move) {
        cout << "send_play()\n";
        string st = move.get_string();
        cout << st << "\n";
    }
};

int main() {
    //ForestRaven forest_raven;
    //forest_raven.start();
    Chess chess;
    set<Move>* moveList = chess.get_candidateMove(chess.myColor);
    for (const Move& move : *moveList) {
        cout << move.get_string();
    }
}