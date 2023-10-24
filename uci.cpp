#include "movegen.h"
#include "chess_utils.h"

// parse user/GUI move string input (e.g. "e7e8q")
int parse_move(char *move_string) {
    moves move_list[1];
    generate_moves(move_list);

    // parse start square
    int start_square = (move_string[0] - 'a') + 16 * (7 - (move_string[1] - '1'));
    // parse end square
    int target_square = (move_string[2] - 'a') + 16 * (7 - (move_string[3] - '1'));

    // loop over moves in move list
    for (int move_count = 0; move_count < move_list->count; ++move_count) {
        int move = move_list->moves[move_count];

        // make sure source and target squares are available within generated move
        if (get_move_start(move) == start_square && get_move_target(move) == target_square) {
            int promoted_piece = get_promoted_piece(move);
            
            // promoted piece is available
            if (promoted_piece) {
                // promoted to queen
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q') {
                    // return legal move
                    return move;
                }
                // promoted to rook
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r') {
                    // return legal move
                    return move;
                }
                // promoted to bishop
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b') {
                    // return legal move
                    return move;
                }
                // promoted to knight
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n') {
                    // return legal move
                    return move;
                }
                // continue the loop on possible wrong promotions
                continue;
            }
            // return legal move
            return move;
        }
    }
    // return illegal move
    return 0;
}