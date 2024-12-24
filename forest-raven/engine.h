#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "board.h"

#define INF int(2e9)

namespace ForestRaven {
    class Engine {
    public:
        Board board;
        Color color_AI;
        const int BASIC_DEPTH = 5;
        int current_depth = BASIC_DEPTH;

        // Queen, Rook, Bishop, Knight, King, Pawn
        const int piece_value[PIECE_TYPE_NB]{ 929, 479, 320, 280, 60000, 100 };
        int PST[PIECE_TYPE_NB][COLOR_NB][SQUARE_NB]{
            // QUEEN
            {
                // WHITE
                {
                   -39, -30, -31, -13, -31, -36, -34, -42,
                   -36, -18,   0, -19, -15, -15, -21, -38,
                   -30,  -6, -13, -11, -16, -11, -16, -27,
                   -14, -15,  -2,  -5,  -1, -10, -20, -22,
                    1, -16,  22,  17,  25,  20, -13,  -6,
                   -2,  43,  32,  60,  72,  63,  43,   2,
                   14,  32,  60, -10,  20,  76,  57,  24,
                    6,   1,  -8,-104,  69,  24,  88,  26
                },
                // BLACK
                {
                    6,   1,  -8,-104,  69,  24,  88,  26,
                   14,  32,  60, -10,  20,  76,  57,  24,
                   -2,  43,  32,  60,  72,  63,  43,   2,
                    1, -16,  22,  17,  25,  20, -13,  -6,
                   -14, -15,  -2,  -5,  -1, -10, -20, -22,
                   -30,  -6, -13, -11, -16, -11, -16, -27,
                   -36, -18,   0, -19, -15, -15, -21, -38,
                   -39, -30, -31, -13, -31, -36, -34, -42
                }
            },
            // ROOK
            {
                // WHITE
                {
                   -30, -24, -18,   5,  -2, -18, -31, -32,
                   -53, -38, -31, -26, -29, -43, -44, -53,
                   -42, -28, -42, -25, -25, -35, -26, -46,
                   -28, -35, -16, -21, -13, -29, -46, -30,
                    0,   5,  16,  13,  18,  -4,  -9,  -6,
                   19,  35,  28,  33,  45,  27,  25,  15,
                   55,  29,  56,  67,  55,  62,  34,  60,
                   35,  29,  33,   4,  37,  33,  56,  50
                },
                // BLACK
                {
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            // BISHOP
            {
                // WHITE
                {
                   -7,   2, -15, -12, -14, -15, -10, -10,
                   19,  20,  11,   6,   7,   6,  20,  16,
                   14,  25,  24,  15,   8,  25,  20,  15,
                   13,  10,  17,  23,  17,  16,   0,   7,
                   25,  17,  20,  34,  26,  25,  15,  10,
                   -9,  39, -32,  41,  52, -10,  28, -14,
                   -11,  20,  35, -42, -39,  31,   2, -22,
                   -59, -78, -82, -76, -23,-107, -37, -50
                },
                // BLACK
                {
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            // KNIGHT
            {
                // WHITE
                {
                   -74, -23, -26, -24, -19, -35, -22, -69,
                   -23, -15,   2,   0,   2,   0, -23, -20,
                   -18,  10,  13,  22,  18,  15,  11, -14,
                   -1,   5,  31,  21,  22,  35,   2,   0,
                   24,  24,  45,  37,  33,  41,  25,  17,
                   10,  67,   1,  74,  73,  27,  62,  -2,
                   -3,  -6, 100, -36,   4,  62,  -4, -14,
                   -66, -53, -75, -75, -10, -55, -58, -70,
                },
                // BLACK
                {
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            // KING
            {
                // WHITE
                {
                   17,  30,  -3, -14,   6,  -1,  40,  18,
                   -4,   3, -14, -50, -57, -18,  13,   4,
                   -47, -42, -43, -79, -64, -32, -29, -32,
                   -55, -43, -52, -28, -51, -47,  -8, -50,
                   -55,  50,  11,  -4, -19,  13,   0, -49,
                   -62,  12, -57,  44, -67,  28,  37, -31,
                   -32,  10,  55,  56,  56,  55,  10,   3,
                    4,  54,  47, -99, -99,  60,  83, -62
                },
                // BLACK
                {
                    4,  54,  47, -99, -99,  60,  83, -62,
                   -32,  10,  55,  56,  56,  55,  10,   3,
                   -62,  12, -57,  44, -67,  28,  37, -31,
                   -55,  50,  11,  -4, -19,  13,   0, -49,
                   -55, -43, -52, -28, -51, -47,  -8, -50,
                   -47, -42, -43, -79, -64, -32, -29, -32,
                   -4,   3, -14, -50, -57, -18,  13,   4,
                   17,  30,  -3, -14,   6,  -1,  40,  18
                }
            },
            // PAWN
            {
                // WHITE
                {
                    0,   0,   0,   0,   0,   0,   0,   0,
                   -31,   8,  -7, -37, -36, -14,   3, -31,
                   -22,   9,   5, -11, -10,  -2,   3, -19,
                   -26,   3,  10,   9,   6,   1,   0, -23,
                   -17,  16,  -2,  15,  14,   0,  15, -13,
                    7,  29,  21,  44,  40,  31,  44,   7,
                   78,  83,  86,  73, 102,  82,  85,  90,
                    0,   0,   0,   0,   0,   0,   0,   0
                },
                // BLACK
                {
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0
                }
            }
        };
        void init_pst() {
            Fori(6) if (i != QUEEN && i != KING) for (Square sq = A1; sq < SQUARE_NB; ++sq)
                PST[i][BLACK][sq] = PST[i][WHITE][(int)((SQUARE_NB - 1) - sq)];
        }

        Engine(Color c) { // for game 
            init();
            init_pst();
            board.init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            color_AI = c;
        }
        Engine(string FEN) { // for puzzle
            init();
            init_pst();
            board.init(FEN);
            color_AI = board.turn;
            play();
        }

        int evaluation_pieceValue() {
            int score[COLOR_NB]{ 0, 0 };
            int bishop[COLOR_NB]{ 0, 0 };
            for (Square sq = A1; sq < SQUARE_NB; ++sq) {
                if (sq_bb(sq) & board.existBB) {
                    Piece_type pt = type_of(board.board[sq]);
                    Color color = color_of(board.board[sq]);
                    if (pt == BISHOP) ++bishop[color];
                    score[color] += piece_value[pt] + PST[pt][color][sq];
                }
            }

            if (bishop[WHITE] >= 2) score[WHITE] += 55;
            if (bishop[BLACK] >= 2) score[BLACK] += 55;

            return score[WHITE] - score[BLACK];
        }
        int evaluation_castling() {
            int score[COLOR_NB]{ 0, 0 };
            for (Color color : {WHITE, BLACK}) {
                if (board.castling_K[color]) score[color] += 50;
                if (board.castling_Q[color]) score[color] += 50;
            }
            return score[WHITE] - score[BLACK];
        }
        int evaluation_pawnStructure() {
            int score[COLOR_NB]{ 0, 0 };
            int pawnCount[COLOR_NB][10]{
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            };
            int idx(1);
            for (Bitboard fileBB : {FileA_BB, FileB_BB, FileC_BB, FileD_BB, FileE_BB, FileF_BB, FileG_BB, FileH_BB}) {
                for (Color color : {WHITE, BLACK})
                    pawnCount[color][idx] += bitCount(fileBB & board.byColorBB[color] & board.byTypeBB[PAWN]);
                ++idx;
            }

            for (int i = 1; i <= 8; ++i) {
                for (const Color& color : { WHITE, BLACK }) {
                    // Isolation Pawn
                    int isolation(0);
                    if (!pawnCount[color][i - 1] && pawnCount[color][i] && !pawnCount[color][i + 1])
                        ++isolation;
                    score[color] -= isolation * 20;

                    // Doubled Pawn
                    if (pawnCount[color][i] >= 2) score[color] -= 20;

                    // Passed Pawns
                    // TODO -> forwarding pass pawn calculation
                    int passed(0);
                    if (!pawnCount[~color][i - 1] && pawnCount[color][i] && !pawnCount[~color][i + 1])
                        ++passed;
                    score[color] += passed * 30;
                }
            }

            return score[WHITE] - score[BLACK];
        }
        int evaluation() {
            auto start = chrono::high_resolution_clock::now();

            int score(0);
            score += evaluation_pieceValue();
            score += evaluation_castling();
            score += evaluation_pawnStructure();

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            ++Eval_ct.first; Eval_ct.second += duration;

            return score;
        }

        int alpha_beta(int alpha, int beta, int depth) {
            if (board.isEnd())
                return (!board.isCheck(board.turn) ? 0 : board.turn == BLACK ? int(1e9) - depth : int(-1e9) + depth);

            if (depth == current_depth) return evaluation();

            int resultEval;
            vector<Move>* legalMoves = board.legal_moves();

            if (board.turn == WHITE) {
                resultEval = -INF;
                for (const Move& move : *legalMoves) {
                    int nextEval = search(move, alpha, beta, depth);
                    resultEval = max(resultEval, nextEval);
                    alpha = max(alpha, nextEval);
                    if (beta <= alpha) break;
                }
            }
            else {
                resultEval = INF;
                for (const Move& move : *legalMoves) {
                    int nextEval = search(move, alpha, beta, depth);
                    resultEval = min(resultEval, nextEval);
                    beta = min(beta, nextEval);
                    if (beta <= alpha) break;
                }
            }
            return resultEval;
        }
        int search(Move move, int alpha, int beta, int depth) {
            //Fori(depth) cout << "-";
            //cout << move_notation(move) << "\n";
            Board temp; temp.copy(board);

            board.play(move);
            int score = alpha_beta(alpha, beta, depth + 1);

            board.copy(temp);
            return score;
        }
        double findTime = 0.0;
        pair<Move, string> findBestMove(int depth = 0) {
            LegalMove_ct = make_pair(0, 0);
            Eval_ct = make_pair(0, 0);
            IsEnd_ct = make_pair(0, 0);
            calAttackBB_ct = make_pair(0, 0);

            auto start = chrono::high_resolution_clock::now();

            vector<Move>* legalMoves = board.legal_moves();

            pair<Move, string> moveData;
            current_depth = BASIC_DEPTH;

            // opening e4 fix
            if (color_AI == WHITE && board.full_move == 1) {
                moveData.first = Move(W_PAWN, E2, E4);
                moveData.second = "e4";
                return moveData;
            }

            while (true) {
                int bestEval = color_AI == WHITE ? -INF : INF;
                int alpha(-INF), beta(INF);
                for (const Move& move : *legalMoves) {
                    int nextEval = search(move, alpha, beta, depth);
                    cout << move_nt(legalMoves, move) << " : " << nextEval << "\n";
                    if ((color_AI == WHITE && bestEval < nextEval) ||
                        (color_AI == BLACK && bestEval > nextEval)) {
                        bestEval = nextEval;
                        moveData.first = move;
                        moveData.second = move_nt(legalMoves, move);
                    }

                    color_AI == WHITE ? alpha = max(alpha, bestEval) : beta = min(beta, bestEval);
                }
                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
                findTime = (double)duration / 1e9;
                if (findTime < 1.0) {
                    ++current_depth;
                    // cout << "++current_depth\n";
                }
                else break;
            }

            return moveData;
        }

        int play(bool isTest = false) {
            cout << fixed; cout.precision(2);
            if (!isTest) {
                int c; cin >> c; string t; getline(cin, t);
                color_AI = (Color)c;
            }
            while (true) {
                if (board.turn == color_AI) {
                    board.print(false);
                    cout << "Current evaluation : " << evaluation() << "\n\n";
                    pair<Move, string> moveData = findBestMove();
                    Move bestMove = moveData.first;
                    cout << "[ " << findTime << " sec / Depth : " << current_depth << " ]\n";
                    cout << " - calAttack function " << calAttackBB_ct.first << ", " << (double)calAttackBB_ct.second / 1e9 << " sec\n";
                    cout << " - LegalMove function " << LegalMove_ct.first << ", " << (double)LegalMove_ct.second / 1e9 << " sec\n";
                    cout << " - Evaluation function " << Eval_ct.first << ", " << (double)Eval_ct.second / 1e9 << " sec\n";
                    cout << " - IsEnd function " << IsEnd_ct.first << ", " << (double)IsEnd_ct.second / 1e9 << " sec\n";
                    cout << "Forest Raven >> " << moveData.second << "\n\n";
                    if (!isTest) SEND_move(bestMove);
                    board.play(bestMove);
                }
                else {
                    board.print(true);
                    cout << "Current evaluation : " << evaluation() << "\n\n";
                    vector<Move>* legalMoves = board.legal_moves();
                    Move move;
                    if (!isTest) move = READ_move(legalMoves);
                    else {
                        while (move.piece == NOPIECE) {
                            cout << "input >> ";
                            string st; cin >> st;
                            if (st == "RESTART") return 1;
                            move = nt_move(legalMoves, st);
                        }
                    }

                    board.play(move);
                    cout << "\n";
                }

                if (board.isEnd()) {
                    cout << "GAME OVER\n";
                    if (!board.isCheck(board.turn)) cout << "DRAW";
                    else cout << (board.turn == BLACK ? "WHITE " : "BLACK ") << "WIN\n";
                    board.print(true);
                    break;
                }
            }
            return 0;
        }
    };
}

#endif  // #ifndef ENGINE_H_INCLUDED