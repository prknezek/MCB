#include "defs.h"
#include <limits>

// board encoding
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

// FEN debug positions
char start_position[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char tricky_position[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
char killer_position[] = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";

// enpassant square
int enpassant = no_sq;

// convert board square indexes to coords
std::string square_to_coords[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "i8", "j8", "k8", "l8", "m8", "n8", "o8", "p8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "i7", "j7", "k7", "l7", "m7", "n7", "o7", "p7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "i6", "j6", "k6", "l6", "m6", "n6", "o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "i5", "j5", "k5", "l5", "m5", "n5", "o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "i4", "j4", "k4", "l4", "m4", "n4", "o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "i3", "j3", "k3", "l3", "m3", "n3", "o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "i2", "j2", "k2", "l2", "m2", "n2", "o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "i1", "j1", "k1", "l1", "m1", "n1", "o1", "p1"
};

// side to move
int side = white;

// ascii pieces
char ascii_pieces[] = ".PNBRQKpnbrqko";
// unicode pieces
std::string unicode_pieces[] = {". ", "♟", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};
// encode ascii pieces
int char_pieces[128];

// (dec 15 => bin 1111 => both sides can castle both dirs)
int castle = 15;
/*
                                castle   move     in      in
                                  right  map      binary  decimal

          white king  moved:      1111 & 1100  =  1100    12
    white king's rook moved:      1111 & 1110  =  1110    14
   white queen's rook moved:      1111 & 1101  =  1101    13

           black king moved:      1111 & 0011  =  0011    3
    black king's rook moved:      1111 & 1011  =  0111    11
   black queen's rook moved:      1111 & 0111  =  1011    7
*/
int castling_rights[128] = {
     7, 15, 15, 15,  3, 15, 15, 11,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    13, 15, 15, 15, 12, 15, 15, 14,  o, o, o, o, o, o, o, o
};

// kings' squares
int king_square[2] = {e1, e8};

// decode promoted pieces
char promoted_pieces[14];

// piece move offsets (offset to end squares from starting square)
int knight_offsets[8] = {33, 31, 18, 14, -33, -31, -18, -14};
int bishop_offsets[4] = {15, 17, -15, -17};
int rook_offsets[4] = {16, -16, 1, -1};
int king_offsets[8] = {16, -16, 1, -1, 15, 17, -15, -17};

// piece values for white and black pieces based on Pesto evaluation
//                        P    N    B    R    Q     K
int piece_value[2][6] = { 82, 337, 365, 477, 1025, 12000, 
                          94, 281, 297, 512,  936, 12000};

// move generation variables
int INF = 50000;
int MATE_VALUE = 49000;
int MATE_SCORE = 48000;
int NEXT_MOVE = 0;
int nodes = 0;

// array to keep track of piece counts
// access with piece_count[side][piece]
int piece_count[2][6] = {};
// array to keep track of the indices of all pieces
// access squares with piece_idx[side][piece]
// there can be up to 10 of each type of piece
// (all 8 pawns promoted to knights or bishops + 2 bishops or knights remaining on the board)
int piece_squares[2][6][10] = {};

// game phase variables
int GAMEPHASEMINEG = 518;
int GAMEPHASEMAXMG = 6192;
int GAMEPHASERANGE = GAMEPHASEMAXMG - GAMEPHASEMINEG;