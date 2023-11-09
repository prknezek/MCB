// headers
#include <stdio.h>
#include <iostream>
#include <limits>
#include <vector>
#include <utility>
#include <algorithm>

#include "chess_utils.h"
#include "search.h"
#include "uci.h"
#include "movegen.h"
#include "evaluate.h"
#include "tt.h"
#include "perft.h"

void initialize_char_pieces() {
    for (int i = 0; i < 128; ++i) {
        char_pieces[i] = e;
    }
    char_pieces['P'] = P;
    char_pieces['N'] = N;
    char_pieces['B'] = B;
    char_pieces['R'] = R;
    char_pieces['Q'] = Q;
    char_pieces['K'] = K;
    char_pieces['p'] = p;
    char_pieces['n'] = n;
    char_pieces['b'] = b;
    char_pieces['r'] = r;
    char_pieces['q'] = q;
    char_pieces['k'] = k;
}

void initialize_promoted_pieces() {
    promoted_pieces[0] = '-';
    promoted_pieces[Q] = 'Q';
    promoted_pieces[R] = 'R';
    promoted_pieces[B] = 'B';
    promoted_pieces[N] = 'N';
    promoted_pieces[q] = 'q';
    promoted_pieces[r] = 'r';
    promoted_pieces[b] = 'b';
    promoted_pieces[n] = 'n';
}

void init_all() {
    // initialize conversion arrays
    initialize_char_pieces();
    initialize_promoted_pieces();

    // init random keys for Transposition table
    init_random_keys();

    // clear hash table
    clear_tt();
}

// main driver
int main() {
    init_all();

    int debug = 0;

    if (debug) {
        // parse fen string
        // char fen[] = "r1k5/P7/5p2/7p/4p3/R3K2P/5PP1/8 b - - 0 45";
        char fen2[] = "r1bq1rk1/pppp1ppp/5n2/4Q3/2B5/2N5/PPP2PPP/R1B1K2R b KQ - 1 10";
        char checkmate[] = "4k3/Q7/8/4K3/8/8/8/8 w - - 1 10";
        parse_fen(start_position);

        // make_move(encode_move(d8, e8, 0, 0, 0, 0, 0), all_moves);
        // cout << evaluate() << endl;
        print_board();
    } else {
        // connect to the GUI
        uci_loop();
    }


    return 0;
}