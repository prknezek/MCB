#include "movegen.h"
#include "search.h"
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

// parse UCI position command
void parse_position(char *command) {
    // shift pointer to the right where next token begins
    command += 9;
    // init pointer to the current character in the command string
    char *current_char = command;

    // parse UCI "startpos" command
    if (strncmp(command, "startpos", 8) == 0) {
        // init chess board with start position
        parse_fen(start_position);
    }
    // parse UCI "fen" command
    else {
        // make sure "fen" command is available within command string
        current_char = strstr(command, "fen");

        // if no "fen" command is in command string
        if (current_char == NULL) {
            // init chess board with start position
            parse_fen(start_position);
        } 
        // found "fen" substring
        else {
            // shift pointer to the right where next token begins
            current_char += 4;
            // parse fen string
            parse_fen(current_char);
        }
    }

    // parse moves after position
    current_char = strstr(command, "moves");
    // moves available
    if (current_char != NULL) {
        // shift pointer to the right where next token begins
        current_char += 6;

        // loop over moves in command string
        while (*current_char) {
            // parse move string
            int move = parse_move(current_char);
            // break out of loop if illegal move
            if (move == 0) {
                break;
            }
            // make move
            make_move(move, all_moves);
            // move current character pointer to end of current move
            while (*current_char && *current_char != ' ') {
                ++current_char;
            }
            // go to next move
            ++current_char;
        }
    }
}

// parse UCI go command
void parse_go(char *command) {
    // init depth
    int depth = -1;
    // init character pointer to current depth arg
    char *current_depth = NULL;
    // handle fixed depth search
    if (current_depth = strstr(command, "depth")) {
        depth = atoi(current_depth + 6);
    } else {
        // different time controls
        depth = 6;
    }

    // search position
    nega_max(depth, -CHECKMATE, CHECKMATE);
}


