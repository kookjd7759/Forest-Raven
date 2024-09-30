#include <string>
#include <map>
#include "board.h"

int board_value[8][8];
const int center = 3;
const int semi_center = 2;
const int normal = 1;


namespace ForestRaven {
    /*
    class Algorithm {
    private:
        const int DEPTH = 2, MAXI_int = 2e9, MINI_int = -2e9;
        Chess chess;
        int evaluate_piece_value(const Chess& ch) { return (chess.pieceValue_wb[ch.myColor] - chess.pieceValue_wb[ch.opColor]); }

        int evaluate(const Chess& ch) {
            int score = evaluate_piece_value(ch);
            return score;
        }

        int minimax_alpha_beta(Chess ch, int depth, int alpha, int beta, bool isWhite) {
            if (depth == 0 || ch.isOver())
                return evaluate(ch);

            if (isWhite) {
                int maxi = MINI_int;
                set<Move>* moveList = chess.get_candidateMove(chess.myColor);
                for (const Move& move : *moveList) {
                    Chess next = chess.clone(); next.play(move);
                    int eval = minimax_alpha_beta(next, depth - 1, alpha, beta, false);
                    maxi = max(maxi, eval);
                    alpha = max(alpha, eval);
                    if (beta <= alpha) break;
                }
                return maxi;
            }
            else {
                int mini = MAXI_int;
                set<Move>* moveList = chess.get_candidateMove(chess.myColor);
                for (const Move& move : *moveList) {
                    Chess next = chess.clone(); next.play(move);
                    int eval = minimax_alpha_beta(next, depth - 1, alpha, beta, false);
                    mini = min(mini, eval);
                    beta = min(beta, eval);
                    if (beta <= alpha) break;
                }
                return mini;
            }
        }

        Move findBestMove() {
            int best_eval = chess.myColor == WHITE ? MINI_int : MAXI_int;
            bool isWhite = chess.myColor == WHITE;
            Move bestMove;
            set<Move>* moveList = chess.get_candidateMove(chess.myColor);
            for (const Move& move : *moveList) {
                cout << "[" << to_notation(move.ori) << " -> " << to_notation(move.dest) << "]  -  ";
                Chess next = chess.clone(); next.play(move);
                int next_eval = minimax_alpha_beta(next, DEPTH, MINI_int, MAXI_int, isWhite);
                cout << best_eval << ", " << next_eval << "\n";
                if (chess.myColor == WHITE ? next_eval > best_eval : next_eval < best_eval) {
                    best_eval = next_eval;
                    bestMove = move;
                }
            }

            return bestMove;
        }

    public:
        Algorithm(const Chess ch) {
            chess = ch;
        }
        Move getBestMove() { return findBestMove(); }
    };
    */

    class Engine {
    private:
        Chess* chess;
        /*
        Move find_nextMove() {
            Algorithm algo(chess);
            Move move = algo.getBestMove();
            return move;
        }
        */
    public:
        Engine() { init(); }
        void init() {
            int color; cin >> color;
            string temp; getline(cin, temp);
            chess = new Chess(Color(color));
        }

        /*
        void start() {
            chess->print();
            while (true) {
                chess->turn == chess->myColor ? my_play() : opponent_play();
                chess->print();
            }
        }
        void opponent_play() {
            string line; getline(cin, line);

            Move move(chess->turn, chess->board[]); move.string_init(line);
            chess->play(move);
        }
        void my_play() {
            Move move = find_nextMove();
            chess->play(move);
            send_play(move);
        }
        void send_play(Move& move) {
            cout << move.get_string() << "\n";
        }
        */
    };
}
/*
class ForestRaven {
private:
    Chess chess;

    Move find_nextMove() {
        Algorithm algo(chess);
        Move move = algo.getBestMove();
        return move;
    }

public:
    ForestRaven() { init(); }

    void init() {
        int color; cin >> color;
        string temp; getline(cin, temp);
        chess.myColor = (color == 0 ? WHITE : BLACK);
        chess.opColor = (color == 0 ? BLACK : WHITE);
    }

    void start() {
        //chess.print_board();
        while (true) {
            chess.turn != chess.myColor ? opponent_play() : play();
            //chess.print_board();
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
        cout << move.get_string() << "\n";
    }
};

void init() {
    // set board_value
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            if (x >= 2 && x <= 5 && y >= 2 && y <= 5) {
                board_value[y][x] = (x >= 3 && x <= 4 && y >= 3 && y <= 4) ? 
                    center : semi_center;;
            }
            else board_value[y][x] = normal;
        }
}
*/

using namespace ForestRaven;

int main() {
    Chess chess(WHITE);
    chess.candidate_moves(WHITE);
}