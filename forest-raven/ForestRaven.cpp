#include <string>
#include <map>
#include <vector>
#include "utility.h"
#include "chess.h"

double board_value[8][8];
const double center = 1.5;
const double semi_center = 1.3;
const double normal = 1.0;

class Algorithm {
private:
    Chess chess;

    double cal_evaluation(const Chess& chess) {
        double score = 0.0;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                score += (double)chess.board[y][x].attack_wb[chess.myColor] * board_value[y][x];
            }
        return score;
    }

    Move findBestMove() {
        /*
        Move selected_move;
        double maxiScore = cal_evaluation(chess);
        set<Move>* moveList = chess.get_candidateMove(chess.myColor);
        for (const Move& move : *moveList) {
            Chess next = chess.clone(); next.play(move);
            double nextScore = cal_evaluation(next);
            if (maxiScore < nextScore) {
                selected_move = move;
                maxiScore = nextScore;
            }
        }
        return selected_move;
        */
        set<Move>* moveList = chess.get_candidateMove(chess.myColor);
        int idx = get_random(0, moveList->size() - 1);
        auto it = moveList->begin();
        std::advance(it, idx);
        return *it;
    }

public:
    Algorithm(const Chess ch) { 
        chess = ch; 
    }
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
        string line; getline(cin, line);
        Move move; move.string_init(line);
        Piece piece = chess.board[move.ori.y][move.ori.x].piece;
        move.piece = piece;
        chess.play(move);
    }
    void play() {
        Move move = find_nextMove();
        chess.play(move);
        send_play(move);
    }
    void send_play(Move& move) {
        string st = move.get_string();
        cout << st << "\n";
    }
};

void init() {
    // set board_value
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            if (x >= 2 && x <= 5 && y >= 2 && y <= 5) {
                if (x >= 3 && x <= 4 && y >= 3 && y <= 4) 
                    board_value[y][x] = center;
                else board_value[y][x] = semi_center;;
            }
            else board_value[y][x] = normal;
        }
}

int main() {
    init();
    ForestRaven forest_raven;
    forest_raven.start();
}