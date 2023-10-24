// headers
#include<stdio.h>
#include <iostream>
#include <limits>
#include <vector>
#include <utility>
#include <algorithm>

#include "chess_utils.h"
#include "search.h"
#include "uci.h"
#include "movegen.h"

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
    // char fen[] = "8/p3N2k/1p6/1P3Np1/P1B1P1Pn/8/5K2/3r3q b - - 3 48";
    // parse_fen(tricky_position);
    // print_board();
    
    // get best next move
    // int start_time = get_time_ms();
    // nega_max(DEPTH, -CHECKMATE, CHECKMATE);
    // cout << "Nodes: " << nodes << endl;
    // cout << "Time: " << get_time_ms() - start_time << "ms" << endl;

    char position[] = "position startpos moves e2e4 e7e5";
    parse_position(position);
    print_board();

    return 0;
}