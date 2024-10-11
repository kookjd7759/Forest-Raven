#include <string>
#include <map>
#include <random>
#include <cstdint>
#include "Bitboard.h"

namespace ForestRaven {
    constexpr int centerValue = 1000, semi_centerValue = 500, normalValue = 100;
    constexpr int piece_value[7] = { 9000,   5000,   3000,   3000,   0000,   1000 };

    class Search {
    private:
        const int DEPTH = 3 , MAXI_int = 2e9, MINI_int = -2e9;
        Chess chess;
        int evaluate_piece_value(const Chess& ch) { 
            int score[COLOR_NB]{0,0};
            for (int i = 0; i < 5; i++) {
                Piece_type pt = Piece_type(i);
                score[WHITE] += ch.pieceCount[i] * piece_value[pt];
                score[BLACK] += ch.pieceCount[i + 8] * piece_value[pt];
            }
            return (score[WHITE] - score[BLACK]);
        }
        int evaluate_piece_activity(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            auto cal = [&](Color c) -> void {
                score[c] += bitCount(ch.pieces(c) & centerBB) * centerValue;
                score[c] += bitCount(ch.pieces(c) & semi_centerBB) * semi_centerValue;
                score[c] += bitCount(ch.pieces(c) & normalBB) * normalValue;
                };
            cal(WHITE), cal(BLACK);
            return (score[WHITE] - score[BLACK]);
        }
        int evaluate_controlSquare(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            auto cal = [&](Color c) -> void {
                score[c] += bitCount(ch.byAttackBB[c] & centerBB) * (centerValue / 2);
                score[c] += bitCount(ch.byAttackBB[c] & semi_centerBB) * (semi_centerValue / 2);
                score[c] += bitCount(ch.byAttackBB[c] & normalBB) * (normalValue / 2);
                };
            cal(WHITE), cal(BLACK);
            return (score[WHITE] - score[BLACK]);
        }

        int evaluate(const Chess& ch) {
            int score_value = evaluate_piece_value(ch);
            int score_activity = evaluate_piece_activity(ch);
            int score_controlSquare = evaluate_controlSquare(ch);
            return score_value + score_activity + score_controlSquare;
        }

        int minimax_alpha_beta(Chess ch, int depth, int alpha, int beta) {
            if (depth == 0 || ch.isOver())
                return evaluate(ch);

            if (ch.turn == WHITE) {
                int maxi = MINI_int;
                vector<Move>* moves = ch.candidate_moves(ch.myColor);
                for (const Move& move : *moves) {
                    Chess next = ch.clone(); next.play(move);
                    int eval = minimax_alpha_beta(next, depth - 1, alpha, beta);
                    maxi = max(maxi, eval);
                    alpha = max(alpha, eval);
                    if (beta <= alpha) break;
                }
                return maxi;
            }
            else {
                int mini = MAXI_int;
                vector<Move>* moves = ch.candidate_moves(ch.myColor);
                for (const Move& move : *moves) {
                    Chess next = ch.clone(); next.play(move);
                    int eval = minimax_alpha_beta(next, depth - 1, alpha, beta);
                    mini = min(mini, eval);
                    beta = min(beta, eval);
                    if (beta <= alpha) break;
                }
                return mini;
            }
        }

        Move findBestMove() {
            Move bestMove;
            int best_eval = chess.myColor == WHITE ? MINI_int : MAXI_int;
            vector<Move>* moves = chess.candidate_moves(chess.myColor);
            cout << "moves.size() : " << moves->size() << "\n";
            for (const Move& move : *moves) {
                cout << "[" << sq_notation(move.ori) << " -> " << sq_notation(move.dest) << "]\n";
                Chess next = chess.clone(); next.play(move);
                int next_eval = minimax_alpha_beta(next, DEPTH, MINI_int, MAXI_int);
                cout << best_eval << ", " << next_eval << "\n";
                if (chess.myColor == WHITE ? next_eval > best_eval : next_eval < best_eval) {
                    best_eval = next_eval;
                    bestMove = move;
                }
            }
            return bestMove;
        }

    public:
        Search(Chess ch) { chess = ch; }
        Move getBestMove() { return findBestMove(); }
    };

    class Engine {
    private:
        Chess* chess;

        Move find_nextMove() {
            Search search(*chess);
            Move move = search.getBestMove();
            return move;
        }

        void opponent_play() {
            cout << "opponent_play()\n";
            string line; getline(cin, line);
            Move move; move.string_init(line);
            chess->play(move);
        }
        void my_play() {
            cout << "my_play()\n";
            Move move = find_nextMove();
            chess->play(move);
            send_play(move);
        }
        void send_play(Move& move) { 
            cout << "SEND " << move.get_string() << "\n"; 
        }
    public:
        Engine() { init(); }
        void init() {
            int c; cin >> c; string t; getline(cin, t);
            chess = new Chess(Color(c));
        }

        void start() {
            chess->print();
            while (true) {
                chess->turn == chess->myColor ? my_play() : opponent_play();
                chess->print();
            }
        }
    };
}

using namespace ForestRaven;


int main() {
    init();
    Engine engine;
    engine.start();
}