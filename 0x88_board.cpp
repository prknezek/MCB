// headers
#include <stdio.h>
#include <iostream>
#include <string>

// FEN debug positions
char start_position[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char tricky_position[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

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
enum pieces {e, P, N, B, R, Q, K, p, n, b, r, q, k, o};

// square encoding
enum squares {
    a8 = 0,   b8, c8, d8, e8, f8, g8, h8,
    a7 = 16,  b7, c7, d7, e7, f7, g7, h7,
    a6 = 32,  b6, c6, d6, e6, f6, g6, h6,
    a5 = 48,  b5, c5, d5, e5, f5, g5, h5,
    a4 = 64,  b4, c4, d4, e4, f4, g4, h4,
    a3 = 80,  b3, c3, d3, e3, f3, g3, h3,
    a2 = 96,  b2, c2, d2, e2, f2, g2, h2,
    a1 = 112, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// castling binary representation
//  bin  dec
// 0001   1   white king can castle king side
// 0010   2   white king can castle queen side
// 0100   4   black king can castle king side
// 1000   8   black king can castle queen side
//
// examples
// 1111       both sides can castle both directions
// 1001       black king => queen side
//            white king => king side

// castling rights
enum castling {KC = 1, QC = 2, kc = 4, qc = 8};

// sides to move
enum sides {white, black};

// ascii pieces
char ascii_pieces[] = ".PNBRQKpnbrqko";

// unicode pieces
string unicode_pieces[] = {". ", "♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

// encode ascii pieces
int char_pieces[128];

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

// side to move
int side = white;

// enpassant square
int enpassant = no_sq;

// castling rights (dec 15 => bin 1111 => both sides can castle both dirs)
int castle = 15;

// convert board square indexes to coords
string square_to_coords[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "i8", "j8", "k8", "l8", "m8", "n8", "o8", "p8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "i7", "j7", "k7", "l7", "m7", "n7", "o7", "p7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "i6", "j6", "k6", "l6", "m6", "n6", "o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "i5", "j5", "k5", "l5", "m5", "n5", "o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "i4", "j4", "k4", "l4", "m4", "n4", "o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "i3", "j3", "k3", "l3", "m3", "n3", "o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "i2", "j2", "k2", "l2", "m2", "n2", "o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "i1", "j1", "k1", "l1", "m1", "n1", "o1", "p1"
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
    cout << "\n   a  b  c  d  e  f  g  h\n\n";

    // print board stats
    cout << "---------------------------" << endl;
    cout << " Side: " << ((side == white) ? "white" : "black") << endl;
    cout << " Castling: " << ((castle & KC) ? 'K' : '-')
                          << ((castle & QC) ? 'Q' : '-')
                          << ((castle & kc) ? 'k' : '-')
                          << ((castle & qc) ? 'q' : '-') << endl;
    cout << " Enpassant: " << ((enpassant == no_sq) ? "-" : square_to_coords[enpassant]) << endl;
}

// reset board
void reset_board() {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 16; ++file) {
            // init square
            int square = rank * 16 + file;

            // if square is on board
            if (!(square & 0x88)) {
                // reset current board square
                board[square] = e;
            }
        }
    }
    // reset stats
    side = -1;
    castle = 0;
    enpassant = no_sq;
}

// parse FEN
void parse_fen(char *fen) {
    // reset board
    reset_board();

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 16; ++file) {
            // init square
            int square = rank * 16 + file;

            // if square is on board
            if (!(square & 0x88)) {
                // match pieces
                if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                    // set the piece on the board
                    board[square] = char_pieces[*fen];

                    // increment FEN pointer to next char
                    *fen++;

                    // cout << "square: " << square_to_coords[square] << " | current FEN char: " << *fen << endl;
                }
                // match empty squares
                if (*fen >= '0' && *fen <= '9') {
                    // calculate offset
                    int offset = *fen - '0';

                    // decrement file on empty squares
                    if (board[square] == e) {
                        file--;
                    }

                    // skip empty squares
                    file += offset;

                    *fen++;
                }
                // match end of rank
                if (*fen == '/') {
                    *fen++;
                }
            }
        }
    }
    // increment FEN pointer to parse side to move
    *fen++;
    side = (*fen == 'w') ? white : black;
    // increment FEN pointer to parse castling rights
    fen += 2;
    while (*fen != ' ') {
        switch(*fen) {
            case 'K' : castle |= KC; break;
            case 'Q' : castle |= QC; break;
            case 'k' : castle |= kc; break;
            case 'q' : castle |= qc; break;
            case '-' : break;
        }
        *fen++;
    }
    // increment FEN pointer to parse enpassant square
    *fen++;
    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        // get enpassant square board index
        enpassant = rank * 16 + file;
    } else {
        enpassant = no_sq;
    }
}

// main driver
int main() {
    initialize_char_pieces();
    
    parse_fen(start_position);
    print_board();
    return 0;
}