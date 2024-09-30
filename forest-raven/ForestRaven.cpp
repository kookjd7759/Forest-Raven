#include <string>
#include <map>
#include <random>
#include <cstdint>
#include "board.h"

namespace ForestRaven {
    constexpr Bitboard centerBB = sq_bb(E4) | sq_bb(E5) | sq_bb(D4) | sq_bb(D5);
    constexpr Bitboard semi_centerBB = sq_bb(C3) | sq_bb(D3) | sq_bb(E3) | sq_bb(F3) | sq_bb(C4) | 
        sq_bb(F4) | sq_bb(C5) | sq_bb(F5) | sq_bb(C6) | sq_bb(D6) | sq_bb(E6) | sq_bb(F6);
    constexpr Bitboard normalBB = ~(centerBB | semi_centerBB);
    constexpr int centerValue = 300, semi_centerValue = 200, normalValue = 100;
    constexpr int piece_value[7] = { 9000,   5000,   3000,   3000,   0000,   1000 };

    int count_bit_temp(Bitboard num) {
        int cnt(0);
        while (num) {
            cnt += num & 1ULL;  // 맨 끝 비트가 1이면 count 증가
            num >>= 1;         // 오른쪽으로 한 비트 이동
        }
        return cnt;
    }

    class Search {
    private:
        const int DEPTH = 4, MAXI_int = 2e9, MINI_int = -2e9;
        Chess chess;
        int evaluate_piece_value(const Chess& ch) { 
            int score[COLOR_NB]{0,0};
            for (int i = 0; i < 5; i++) {
                Piece_type pt = init_positions[i];
                score[WHITE] += count_bit_temp(ch.byTypeBB[pt] & ch.byColorBB[WHITE]) * piece_value[pt];
                score[BLACK] += count_bit_temp(ch.byTypeBB[pt] & ch.byColorBB[BLACK]) * piece_value[pt];
            }
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
        int evaluate_controlSquare(const Chess& ch) {
            int score[COLOR_NB]{ 0,0 };
            
            score[WHITE] += count_bit_temp(centerBB & ch.byColorBB[WHITE]) * centerValue;
            score[WHITE] += count_bit_temp(semi_centerBB & ch.byColorBB[WHITE]) * semi_centerValue;
            score[WHITE] += count_bit_temp(normalBB & ch.byColorBB[WHITE]) * normalValue;

            score[BLACK] += count_bit_temp(centerBB & ch.byColorBB[BLACK]) * centerValue;
            score[BLACK] += count_bit_temp(semi_centerBB & ch.byColorBB[BLACK]) * semi_centerValue;
            score[BLACK] += count_bit_temp(normalBB & ch.byColorBB[BLACK]) * normalValue;

            return (score[WHITE] - score[BLACK]);
        }

        int evaluate(const Chess& ch) {
            int score_value = evaluate_piece_value(ch);
            int score_threat = evaluate_threat(ch);
            int score_controlSquare = evaluate_controlSquare(ch);
            return score_value + score_threat + score_controlSquare;
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