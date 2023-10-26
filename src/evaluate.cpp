#include "chess_utils.h"
#include "evaluate.h"


// score the board based on material and piece-square tables
int evaluate() {
    int score = 0;
    // loop over all board squares
    for (int square = 0; square < 128; ++square) {
        // if square is on board
        if (on_board(square)) {
            // if square is not empty
            if (board[square] != e) {
                if (is_white_piece(board[square])) {
                    // add piece value to score
                    score += piece_value[board[square]];
                } else {
                    // subtract piece value from score
                    score -= piece_value[board[square]];
                }
            }
        }
    }
    return score * (side == white ? 1 : -1);
}