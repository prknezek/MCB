#ifndef CHESSDEFS_H
#define CHESSDEFS_H

#include<iostream>
#include<string.h>

using std::string;
using std::cout;
using std::endl;

// piece encoding

// each letter or "piece" in enum list gets a value from 0 - 13 
// based on its index Ex: e = 0, q = 11

// e = empty square
// *uppercase* = white piece
// *lowercase* = black piece
// o = off board square
enum pieces {e, P, N, B, R, Q, K, p, n, b, r, q, k, o};

extern int board[128];


extern char start_position[];
extern char tricky_position[];
extern char killer_position[];

// square encoding
// no_sq is reserved for the enpassant square
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

extern int enpassant;

extern std::string square_to_coords[];

// capture flags
enum capture_flags { all_moves, only_captures };

// sides to move definition
enum sides {white, black};

extern int side;

extern char ascii_pieces[];
extern std::string unicode_pieces[];
extern int char_pieces[128];

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
extern int castle;
extern int castling_rights[128];

extern int king_square[2];

extern char promoted_pieces[14];

extern int knight_offsets[8];
extern int bishop_offsets[4];
extern int rook_offsets[4];
extern int king_offsets[8];

extern int piece_value[2][6];

extern int CHECKMATE;
extern int NEXT_MOVE;
extern int nodes;

extern int piece_count[2][6];
extern int piece_squares[2][6][10];
extern int GAMEPHASEMINEG;
extern int GAMEPHASEMAXMG;
extern int GAMEPHASERANGE;

#endif