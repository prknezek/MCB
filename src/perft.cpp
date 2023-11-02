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
        // copy board state
        copy_board();

        // if we make an illegal move skip it
        if(!make_move(move_list->moves[i], all_moves)) {
            continue;
        }
        // make recursive call
        nodes += perft_driver(depth - 1);

        // restore board position
        restore_board();
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