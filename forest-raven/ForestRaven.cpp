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
        const int DEPTH = 4, MAXI_int = 2e9, MINI_int = -2e9;
        Chess chess;
        int evaluate_piece_value(const Chess& ch) { 
            int score[COLOR_NB]{0,0};
            for (int i = 0; i < 5; i++) {
                Piece_type pt = initPos[i];
                score[WHITE] += bitCount(ch.byTypeBB[pt] & ch.byColorBB[WHITE]) * piece_value[pt];
                score[BLACK] += bitCount(ch.byTypeBB[pt] & ch.byColorBB[BLACK]) * piece_value[pt];
            }
            return (score[WHITE] - score[BLACK]);
        }
        int evaluate_piece_activity(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            auto cal = [&](Color c, Piece_type pt) -> void {
                score[c] += bitCount(ch.byTypeBB[pt] & ch.byColorBB[c] & centerBB) * (piece_value[pt] + centerValue);
                score[c] += bitCount(ch.byTypeBB[pt] & ch.byColorBB[c] & semi_centerBB) * (piece_value[pt] + semi_centerValue);
                score[c] += bitCount(ch.byTypeBB[pt] & ch.byColorBB[c] & normalBB) * (piece_value[pt] + normalValue);
                };
            for (int i = 0; i < 5; i++) {
                Piece_type pt = initPos[i];
                cal(WHITE, pt);
                cal(BLACK, pt);
            }
            return (score[WHITE] - score[BLACK]);
        }
        int evaluate_controlSquare(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            auto cal = [&](Color c) -> void {
                score[WHITE] += bitCount(centerBB & ch.byColorBB[WHITE]) * centerValue;
                score[WHITE] += bitCount(semi_centerBB & ch.byColorBB[WHITE]) * semi_centerValue;
                score[WHITE] += bitCount(normalBB & ch.byColorBB[WHITE]) * normalValue;
                };
            cal(WHITE), cal(BLACK);
            return (score[WHITE] - score[BLACK]);
        }
        int evaluate_threat(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            for (Square s = A1; s <= H8; ++s) {
                Bitboard b = sq_bb(s);
                if (b & ch.existBB) { // piece exist
                    if (b & ch.byColorBB[WHITE]) { // WHITE color piece
                        if (b & ch.byAttackBB[BLACK] && !(b & ch.byAttackBB[WHITE])) // Attacked and not protected
                            score[BLACK] += piece_value[ch.board[s]];
                    }
                    else { // BLACK color piece
                        if (b & ch.byAttackBB[WHITE] && !(b & ch.byAttackBB[BLACK])) // Attacked and not protected
                            score[WHITE] += piece_value[ch.board[s]];
                    }
                }
            }
            return (score[WHITE] - score[BLACK]);
        }

        int evaluate(const Chess& ch) {
            int score_value = evaluate_piece_value(ch);
            int score_activity = evaluate_piece_activity(ch);
            int score_threat = evaluate_threat(ch);
            int score_controlSquare = evaluate_controlSquare(ch);
            return score_value + score_activity + score_threat + score_controlSquare;
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
        void send_play(Move& move) { cout << "SEND " << move.get_string() << "\n"; }
    public:
        Engine() { init(); }
        void init() {
            int c; cin >> c; string t; getline(cin, t);
            chess = new Chess(Color(c));
        }

        void start() {
            chess->print();
            while (true) {
                cout << chess->turn << "\n";
                cout << chess->myColor << "\n";
                chess->turn == chess->myColor ? my_play() : opponent_play();
                chess->print();
            }
        }
    };
}

using namespace ForestRaven;


int main() {
    Engine engine;
    engine.start();
}