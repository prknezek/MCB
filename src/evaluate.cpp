#include "chess_utils.h"
#include "evaluate.h"

// get the opposite square so we can use one table for white and black
#define FLIP(sq) (119 - (sq))

int pawn_mg[128] = {0,   0,   0,   0,   0,   0,  0,   0, o, o, o, o, o, o, o, o,
                    98, 134,  61,  95,  68, 126, 34, -11, o, o, o, o, o, o, o, o,
                    -6,   7,  26,  31,  65,  56, 25, -20, o, o, o, o, o, o, o, o,
                    -14,  13,   6,  21,  23,  12, 17, -23, o, o, o, o, o, o, o, o,
                    -27,  -2,  -5,  12,  17,   6, 10, -25, o, o, o, o, o, o, o, o,
                    -26,  -4,  -4, -10,   3,   3, 33, -12, o, o, o, o, o, o, o, o,
                    -35,  -1, -20, -23, -15,  24, 38, -22, o, o, o, o, o, o, o, o,
                    0,   0,   0,   0,   0,   0,  0,   0, o, o, o, o, o, o, o, o,
};

int knight_mg[128] = {-167, -89, -34, -49,  61, -97, -15, -107, o, o, o, o, o, o, o, o,
                      -73, -41,  72,  36,  23,  62,   7,  -17, o, o, o, o, o, o, o, o,
                      -47,  60,  37,  65,  84, 129,  73,   44, o, o, o, o, o, o, o, o,
                      -9,  17,  19,  53,  37,  69,  18,   22, o, o, o, o, o, o, o, o,
                      -13,   4,  16,  13,  28,  19,  21,   -8, o, o, o, o, o, o, o, o,
                      -23,  -9,  12,  10,  19,  17,  25,  -16, o, o, o, o, o, o, o, o,
                      -29, -53, -12,  -3,  -1,  18, -14,  -19, o, o, o, o, o, o, o, o,
                      -105, -21, -58, -33, -17, -28, -19,  -23, o, o, o, o, o, o, o, o,
};

int bishop_mg[128] = {-29,   4, -82, -37, -25, -42,   7,  -8, o, o, o, o, o, o, o, o,
                      -26,  16, -18, -13,  30,  59,  18, -47, o, o, o, o, o, o, o, o,
                      -16,  37,  43,  40,  35,  50,  37,  -2, o, o, o, o, o, o, o, o,
                      -4,   5,  19,  50,  37,  37,   7,  -2, o, o, o, o, o, o, o, o,
                      -6,  13,  13,  26,  34,  12,  10,   4, o, o, o, o, o, o, o, o,
                      0,  15,  15,  15,  14,  27,  18,  10, o, o, o, o, o, o, o, o,
                      4,  15,  16,   0,   7,  21,  33,   1, o, o, o, o, o, o, o, o,
                      -33,  -3, -14, -21, -13, -12, -39, -21, o, o, o, o, o, o, o, o,
};

int rook_mg[128] = {32,  42,  32,  51, 63,  9,  31,  43, o, o, o, o, o, o, o, o,
                    27,  32,  58,  62, 80, 67,  26,  44, o, o, o, o, o, o, o, o,
                    -5,  19,  26,  36, 17, 45,  61,  16, o, o, o, o, o, o, o, o,
                    -24, -11,   7,  26, 24, 35,  -8, -20, o, o, o, o, o, o, o, o,
                    -36, -26, -12,  -1,  9, -7,   6, -23, o, o, o, o, o, o, o, o,
                    -45, -25, -16, -17,  3,  0,  -5, -33, o, o, o, o, o, o, o, o,
                    -44, -16, -20,  -9, -1, 11,  -6, -71, o, o, o, o, o, o, o, o,
                    -19, -13,   1,  17, 16,  7, -37, -26, o, o, o, o, o, o, o, o,
};

int queen_mg[128] = {-28,   0,  29,  12,  59,  44,  43,  45, o, o, o, o, o, o, o, o,
                     -24, -39,  -5,   1, -16,  57,  28,  54, o, o, o, o, o, o, o, o,
                     -13, -17,   7,   8,  29,  56,  47,  57, o, o, o, o, o, o, o, o,
                     -27, -27, -16, -16,  -1,  17,  -2,   1, o, o, o, o, o, o, o, o,
                     -9, -26,  -9, -10,  -2,  -4,   3,  -3, o, o, o, o, o, o, o, o,
                     -14,   2, -11,  -2,  -5,   2,  14,   5, o, o, o, o, o, o, o, o,
                     -35,  -8,  11,   2,   8,  15,  -3,   1, o, o, o, o, o, o, o, o,
                     -1, -18,  -9,  10, -15, -25, -31, -50, o, o, o, o, o, o, o, o,
};

int king_mg[128] = {-65,  23,  16, -15, -56, -34,   2,  13, o, o, o, o, o, o, o, o,
                    29,  -1, -20,  -7,  -8,  -4, -38, -29, o, o, o, o, o, o, o, o,
                    -9,  24,   2, -16, -20,   6,  22, -22, o, o, o, o, o, o, o, o,
                    -17, -20, -12, -27, -30, -25, -14, -36, o, o, o, o, o, o, o, o,
                    -49,  -1, -27, -39, -46, -44, -33, -51, o, o, o, o, o, o, o, o,
                    -14, -14, -22, -46, -44, -30, -15, -27, o, o, o, o, o, o, o, o,
                    1,   7,  -8, -64, -43, -16,   9,   8, o, o, o, o, o, o, o, o,
                    -15,  36,  12, -54,   8, -28,  24,  14, o, o, o, o, o, o, o, o,
};

int *tables[6] = {pawn_mg, knight_mg, bishop_mg, rook_mg, queen_mg, king_mg};

// score the board based on material and piece-square tables
int evaluate() {
    int score = 0;
    // loop over all board squares
    for (int square = 0; square < 128; ++square) {
        // if square is on board
        if (on_board(square)) {
            // if square is not empty
            int piece = board[square];
            if (piece != e) {
                if (is_white_piece(piece)) {
                    // add piece value to score
                    score += piece_value[0][piece-1] + tables[piece-1][square];
                } else {
                    // subtract piece value from score
                    score += -piece_value[1][piece-7] - tables[piece-7][FLIP(square)];
                }
            }
        }
    }
    return score * (side == white ? 1 : -1);
}

int test_square(int square) {
    if (on_board(square)) {
        int piece = board[square];
        if (piece != e) {
            if (is_white_piece(piece)) {
                // add piece value to score
                return piece_value[0][piece - 1] + tables[piece-1][FLIP(square)];
            } else {
                // subtract piece value from score
                return -piece_value[1][piece - 7] - tables[piece-7][square];
            } 
        }
    }
    return 0;
}