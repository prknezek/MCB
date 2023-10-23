#include "chess_utils.h"
#include "movegen.h"

// new perft driver
int perft_driver(int depth = 1) {
    // create move_list instance
    moves move_list[1];
    long nodes = 0;

    // base case goes to depth 1 because number of moves generated at depth 1
    // is just the number of moves available
    if (depth == 0) {
        return 1;
    }

    // generate moves
    generate_moves(move_list);

    // go through all valid moves
    for (int i = 0; i < move_list->count; ++i) {
        // create board state copy variables
        int board_copy[128], king_square_copy[2];
        int side_copy, enpassant_copy, castle_copy;

        // copy board state
        memcpy(board_copy, board, 512);
        side_copy = side, enpassant_copy = enpassant, castle_copy = castle;
        memcpy(king_square_copy, king_square, 8);

        // if we make an illegal move skip it
        if(!make_move(move_list->moves[i], all_moves)) {
            continue;
        }
        // make recursive call
        nodes += perft_driver(depth - 1);

        // restore board position
        memcpy(board, board_copy, 512);
        side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
        memcpy(king_square, king_square_copy, 8);
    }
    return nodes;
}

// perft test
void perft_test(int depth) {
    cout << "\n Performance Test\n";

    // init start time and nodes
    int start_time = get_time_ms();
    long nodes = 0;

    // recursive call
    nodes = perft_driver(depth);

    // print results
    cout << "\n Depth: " << depth << endl;
    cout << " Nodes: " << nodes << endl;
    cout << " Time: " << get_time_ms() - start_time << "ms" << endl;
}