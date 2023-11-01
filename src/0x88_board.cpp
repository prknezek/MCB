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

// main driver
int main() {
    // initialize conversion arrays
    initialize_char_pieces();
    initialize_promoted_pieces();

    // parse fen string
    char fen[] = "rn1k1bnr/pp3ppp/4p3/2pN4/6b1/5N2/PPP1PPPP/R1B1KB1R w KQ - 3 6";
    parse_fen(fen);
    print_board();
    
    // get best next move
    int start_time = get_time_ms();
    DEPTH = 1;

    nega_max(DEPTH, -CHECKMATE, CHECKMATE);
    cout << "Nodes: " << nodes << endl;
    cout << "Time: " << get_time_ms() - start_time << "ms" << endl;
    print_move(NEXT_MOVE);

    cout << evaluate() << endl;
    make_move(encode_move(d5, f6, 0, 0, 0, 0, 0), all_moves);
    cout << evaluate() << endl;

    // connect to the GUI
    // uci_loop();
    //cout << test_square(b6) << endl;
    // randint();
    // cout << evaluate() << endl;

    return 0;
}