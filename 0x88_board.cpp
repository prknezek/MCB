// headers
#include <stdio.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

// piece encoding

// each letter or "piece" in enum list gets a value from 0 - 13 
// based on its index Ex: e = 0, q = 11

// e = empty square
// *uppercase* = white piece
// *lowercase* = black piece
// o = off board scores
enum {e, P, N, B, R, Q, K, p, n, b, r, q, k, o};

// ascii pieces
char ascii_pieces[] = ".PNBRQKpnbrqko";

// unicode pieces
string unicode_pieces[] = {".  ", "♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

// chess board representation
int board[128] = {
    r, n, b, q, k, b, n, r,  o, o, o, o, o, o, o, o,
    p, p, p, p, p, p, p, p,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    P, P, P, P, P, P, P, P,  o, o, o, o, o, o, o, o,
    R, N, B, Q, K, B, N, R,  o, o, o, o, o, o, o, o
};

// print board
void print_board() {
    // print indentation
    cout << endl;

    // loop over board ranks
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 16; ++file) {
            // init square
            int square = rank * 16 + file;

            // print ranks
            if (file == 0) {
                cout << 8 - rank << "  ";
            }

            // if square is on board
            if (!(square & 0x88)) {
                //cout << ascii_pieces[board[square]] << " ";
                cout << unicode_pieces[board[square]] << " ";
            }
        }
        // print new line every time new rank is encountered
        cout << endl;
    }
    // print files
    cout << "\n   a b c d e f g h\n";
}

// main driver
int main() {
    print_board();
    return 0;
}