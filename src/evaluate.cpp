#include "chess_utils.h"
#include "evaluate.h"

// get the opposite square so we can use one table for white and black
#define FLIP(sq) (119 - (sq))

// encourage pawns to move towards center of the board and promote
// pawns are also encouraged to not be pushed in front of king when castled
int pawn_mg[128] = {0,  0,  0,  0,  0,  0,  0,  0, o, o, o, o, o, o, o, o,
                   50, 50, 50, 50, 50, 50, 50, 50, o, o, o, o, o, o, o, o,
                   10, 10, 20, 30, 30, 20, 10, 10, o, o, o, o, o, o, o, o,
                    5,  5, 10, 25, 25, 10,  5,  5, o, o, o, o, o, o, o, o,
                    0,  0,  0, 30, 35,  0,  0,  0, o, o, o, o, o, o, o, o,
                    5, -5,-10,  0,  0,-10, -5,  5, o, o, o, o, o, o, o, o,
                    5, 10, 10,-20,-20, 10, 10,  5, o, o, o, o, o, o, o, o,
                    0,  0,  0,  0,  0,  0,  0,  0, o, o, o, o, o, o, o, o};

// encourage knights to move towards center of the board
int knight_mg[128] = {-50,-30,-30,-30,-30,-30,-30,-50, o, o, o, o, o, o, o, o,
                      -40,-20,  0,  0,  0,  0,-20,-40, o, o, o, o, o, o, o, o,
                      -30,  0, 10, 15, 15, 10,  0,-30, o, o, o, o, o, o, o, o,
                      -30,  5, 15, 20, 20, 15,  5,-30, o, o, o, o, o, o, o, o,
                      -30,  0, 15, 20, 20, 15,  0,-30, o, o, o, o, o, o, o, o,
                      -30,  5, 10, 15, 15, 10,  5,-30, o, o, o, o, o, o, o, o,
                      -40,-20,  0,  5,  5,  0,-20,-40, o, o, o, o, o, o, o, o,
                      -50,-30,-30,-30,-30,-30,-30,-50, o, o, o, o, o, o, o, o,};

// bishops are encouraged to avoid corners and borders
// bishops are also encouraged to control diagonals
int bishop_mg[128] = {-20,-10,-10,-10,-10,-10,-10,-20, o, o, o, o, o, o, o, o,
                      -10,  0,  0,  0,  0,  0,  0,-10, o, o, o, o, o, o, o, o,
                      -10,  0,  5, 10, 10,  5,  0,-10, o, o, o, o, o, o, o, o,
                      -10,  5,  5, 10, 10,  5,  5,-10, o, o, o, o, o, o, o, o,
                      -10,  0, 10, 10, 10, 10,  0,-10, o, o, o, o, o, o, o, o,
                      -10, 10, 10, 10, 10, 10, 10,-10, o, o, o, o, o, o, o, o,
                      -10,  5,  0,  0,  0,  0,  5,-10, o, o, o, o, o, o, o, o,
                      -20,-10,-10,-10,-10,-10,-10,-20, o, o, o, o, o, o, o, o,};

// rooks are encouraged to castle, go to 7th rank, and stay away from borders
int rook_mg[128] = {0,  0,  0,  0,  0,  0,  0,  0, o, o, o, o, o, o, o, o,
                    5, 10, 10, 10, 10, 10, 10,  5, o, o, o, o, o, o, o, o,
                   -5,  0,  0,  0,  0,  0,  0, -5, o, o, o, o, o, o, o, o,
                   -5,  0,  0,  0,  0,  0,  0, -5, o, o, o, o, o, o, o, o,
                   -5,  0,  0,  0,  0,  0,  0, -5, o, o, o, o, o, o, o, o,
                   -5,  0,  0,  0,  0,  0,  0, -5, o, o, o, o, o, o, o, o,
                   -5,  0,  0,  0,  0,  0,  0, -5, o, o, o, o, o, o, o, o,
                    0,  0,  0,  5,  5,  0,  0,  0, o, o, o, o, o, o, o, o};

// queen stays away from border to attack more squares
int queen_mg[128] = {-20,-10,-10, -5, -5,-10,-10,-20, o, o, o, o, o, o, o, o,
                    -10,  0,  0,  0,  0,  0,  0,-10, o, o, o, o, o, o, o, o,
                    -10,  0,  5,  5,  5,  5,  0,-10, o, o, o, o, o, o, o, o,
                    -5,  0,  5,  5,  5,  5,  0, -5, o, o, o, o, o, o, o, o,
                     0,  0,  5,  5,  5,  5,  0, -5, o, o, o, o, o, o, o, o,
                    -10,  5,  5,  5,  5,  5,  0,-10, o, o, o, o, o, o, o, o,
                    -10,  0,  5,  0,  0,  0,  0,-10, o, o, o, o, o, o, o, o,
                    -20,-10,-10, -5, -5,-10,-10,-20, o, o, o, o, o, o, o, o};

// encourage king to stand behind pawn shelter (castle)
int king_mg[128] = {-50,-40,-30,-20,-20,-30,-40,-50, o, o, o, o, o, o, o, o,
                    -30,-20,-10,  0,  0,-10,-20,-30, o, o, o, o, o, o, o, o,
                    -30,-10, 20, 30, 30, 20,-10,-30, o, o, o, o, o, o, o, o,
                    -30,-10, 30, 40, 40, 30,-10,-30, o, o, o, o, o, o, o, o,
                    -30,-10, 30, 40, 40, 30,-10,-30, o, o, o, o, o, o, o, o,
                    -30,-10, 20, 30, 30, 20,-10,-30, o, o, o, o, o, o, o, o,
                    -30,-30,  0,  0,  0,  0,-30,-30, o, o, o, o, o, o, o, o,
                    -50,-30,-30,-30,-30,-30,-30,-50, o, o, o, o, o, o, o, o };

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
                    score += piece_value[piece] + tables[piece-1][square];
                } else {
                    // subtract piece value from score
                    score += -piece_value[piece] - tables[piece-7][FLIP(square)];
                }
            }
        }
    }
    return score * (side == white ? 1 : -1);
}

int test(int square) {
    if (on_board(square)) {
        int piece = board[square];
        if (piece != e) {
            if (is_white_piece(piece)) {
                // add piece value to score
                return piece_value[piece] + tables[piece-1][square];
            } else {
                // subtract piece value from score
                return -piece_value[piece] - tables[piece-7][FLIP(square)];
            } 
        }
    }
    return 0;
}