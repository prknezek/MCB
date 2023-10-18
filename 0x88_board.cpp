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
string unicode_pieces[] = {". ", "♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

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

/*
    Move formatting
    
    0000 0000 0000 0000 0111 1111       start square            0x7f
    0000 0000 0011 1111 1000 0000       target square           0x3f80
    0000 0011 1100 0000 0000 0000       promoted piece
                                        flags:
    0000 0100 0000 0000 0000 0000           capture
    0000 1000 0000 0000 0000 0000           double pawn move
    0001 0000 0000 0000 0000 0000           enpassant
    0010 0000 0000 0000 0000 0000           castling
*/

// turn a move into an integer
#define encode_move(source, target, piece, capture_f, pawn_f, enpassant_f, castling_f) \
(                           \
    (source) |              \
    (target << 7) |         \
    (piece << 14) |         \
    (capture_f << 18) |     \
    (pawn_f << 19) |        \
    (enpassant_f << 20) |   \
    (castling_f << 21)      \
)

// decode move's start square
#define get_move_start(move) (move & 0x7f)

// decode move's target square
#define get_move_target(move) ((move >> 7) & 0x7f)

// decode move's promoted piece
#define get_promoted_piece(move) ((move >> 14) & 0xf)

// decode move's capture flag
#define get_move_capture(move) ((move >> 18) & 0x1)

// decode move's double pawn push flag
#define get_move_pawn(move) ((move >> 19) & 0x1)

// decode move's enpassant flag
#define get_move_enpassant(move) ((move >> 20) & 0x1)

// decode move's castling flag
#define get_move_castling(move) ((move >> 21) & 0x1)

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

// decode promoted pieces
char promoted_pieces[14];

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

// piece move offsets (offset to end squares from starting square)
int knight_offsets[8] = {33, 31, 18, 14, -33, -31, -18, -14};
int bishop_offsets[4] = {15, 17, -15, -17};
int rook_offsets[4] = {16, -16, 1, -1};
int king_offsets[8] = {16, -16, 1, -1, 15, 17, -15, -17};

// move list structure
typedef struct {
    // move list
    int moves[256];
    // move count
    int count = 0;
} moves;

// determines if a square is on the board or not
bool on_board(int square) {
    return (!(square & 0x88));
}

// determines if a square is empty or not
bool is_empty_square(int square) {
    return (board[square] == e);
}

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
            if (on_board(square)) {
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
            if (on_board(square)) {
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

// returns if the given square is attacked
int is_square_attacked(int square, int side) {
    // pawn attacks
    if (side == white) {
        // if target square is on board and white pawn
        // left pawn attack
        if (on_board(square + 17) && (board[square + 17] == P)) {
            return 1;
        }
        // right pawn attack
        if (on_board(square + 15) && (board[square + 15] == P)) {
            return 1;
        }
    } else {
        // if target square is on board and black pawn
        // right pawn attack
        if (on_board(square - 17) && (board[square - 17] == p)) {
            return 1;
        }
        // left pawn attack
        if (on_board(square - 15) && (board[square - 15] == p)) {
            return 1;
        }
    }

    // knight attacks
    for (int i = 0; i < 8; ++i) {
        int target_square = square + knight_offsets[i];

        // lookup target piece
        int target_piece = board[target_square];

        // if target square is on board
        if (on_board(target_square)) {
            // determine if white or black knight is targeting a square (depending on turn)
            if (side == white ? target_piece == N : target_piece == n) {
                return 1;
            }
        }
    }

    // king attacks
    for (int i = 0; i < 8; ++i) {
        int target_square = square + king_offsets[i];

        // lookup target piece
        int target_piece = board[target_square];

        // if target square is on board
        if (on_board(target_square)) {
            // determine if white or black king is targeting a square (depending on turn)
            if (side == white ? target_piece == K : target_piece == k) {
                return 1;
            }
        }
    }

    // bishop and queen diagonal attacks
    for (int i = 0; i < 4; ++i) {
        int target_square = square + bishop_offsets[i];

        // loop over attack ray
        while (on_board(target_square)) {
            int target_piece = board[target_square];

            // determine if white or black bishop or queen is targeting a square (depending on turn)
            if (side == white ? (target_piece == B || target_piece == Q) :
                                (target_piece == b || target_piece == q)) {
                return 1;
            }

            // break if hit a piece
            if (target_piece != e) {
                break;
            }

            // increment target square by move offset
            target_square += bishop_offsets[i];
        }
    }

    // rook and queen straight attacks
    for (int i = 0; i < 4; ++i) {
        int target_square = square + rook_offsets[i];

        // loop over attack ray
        while (on_board(target_square)) {
            int target_piece = board[target_square];

            // determine if white or black bishop or queen is targeting a square (depending on turn)
            if (side == white ? (target_piece == R || target_piece == Q) :
                                (target_piece == r || target_piece == q)) {
                return 1;
            }

            // break if hit a piece
            if (target_piece != e) {
                break;
            }

            // increment target square by move offset
            target_square += rook_offsets[i];
        }
    }

    return 0;
}

void print_attacked_squares(int side) {
    // print indentation
    cout << endl;
    cout << "   Attacking side: " << (side == white ? "white" : "black") << endl;

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
            if (on_board(square)) {
                cout << (is_square_attacked(square, side) ? "x " : ". ");
            }
        }
        // print new line every time new rank is encountered
        cout << endl;
    }
    cout << "\n   a b c d e f g h\n\n";
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
            if (on_board(square)) {
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
                    if (is_empty_square(square)) {
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


// returns true if piece is a white piece
bool is_white_piece(int piece) {
    return piece >= P && piece <= K;
}

// returns true if piece is a black piece
bool is_black_piece(int piece) {
    return piece >= p && piece <= k;
}

// returns true if piece on square is a black piece
// DOES NOT CHECK IF SQUARE IS ON BOARD
bool is_black_piece_square(int square) {
    return board[square] >= p && board[square] <= k;
}

// returns true if piece on square is a white piece
// DOES NOT CHECK IF SQUARE IS ON BOARD
bool is_white_piece_square(int square) {
    return board[square] >= P && board[square] <= K;
}

// determines if a square is on the 7th rank
// (second farthest rank from white)
bool rank_7(int square) {
    return (square >= a7 && square <= h7);
}

// determines if a square is on the 2nd rank
// (second farthest rank from black)
bool rank_2(int square) {
    return (square >= a2 && square <= h2);
}

// populate move_list
void add_move(moves *move_list, int move) {
    // push move into the move list
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

// move generator
void generate_moves(moves *move_list) {
    // reset move count
    move_list->count = 0;
    // loop over all board squares
    for (int square = 0; square < 128; ++square) {
        if (on_board(square)) {
            // white pawn and castling moves
            if (side == white) {
                // white pawn moves
                if (board[square] == P) {
                    int target_square = square - 16;
                    // quiet pawn moves (moves that aren't capturing)
                    // check if target square is on board
                    if (on_board(target_square) && is_empty_square(target_square)) {
                        // pawn promotions (make sure pawns are on 7th rank)
                        if (rank_7(square)) {
                            add_move(move_list, encode_move(square, target_square, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, N, 0, 0, 0, 0));
                        } else {
                            // one square ahead pawn move
                            add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                            // two squares ahead pawn move (make sure pawns are on 2nd rank)
                            if (rank_2(square) && is_empty_square(square - 32)) {
                                add_move(move_list, encode_move(square, target_square, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    // white pawn captures (using bishop offsets b/c they're the same as pawns)
                    for (int i = 0; i < 4; ++i) {
                        // init pawn offset
                        int pawn_offset = bishop_offsets[i];
                        
                        // white pawn offsets
                        if (pawn_offset < 0) {
                            int target_square = square + pawn_offset;
                            // check if target square is on board
                            if (on_board(target_square)) {
                                // capture pawn promotion
                                if (rank_7(square) && is_black_piece_square(target_square)) {
                                    add_move(move_list, encode_move(square, target_square, Q, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, R, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, B, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, N, 1, 0, 0, 0));
                                } else {
                                    // normal capture
                                    if (is_black_piece_square(target_square)) {
                                        add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                                    }
                                    // enpassant capture
                                    if (target_square == enpassant) {
                                        add_move(move_list, encode_move(square, target_square, 0, 1, 0, 1, 0));
                                    }
                                }
                            }
                        }
                    }
                }
                // white king castling
                if (board[square] == K) {
                    // if king side castling is available
                    if (castle & KC) {
                        // make sure there are empty squares between king & rook
                        if (board[f1] == e && board[g1] == e) {
                            // make sure king & next squares are not under attack
                            if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black)) {
                                add_move(move_list, encode_move(e1, g1, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                    // if queen side castling is available
                    if (castle & QC) {
                        // make sure there are empty squares between king & rook
                        if (board[b1] == e && board[c1] == e && board[d1] == e) {
                            // make sure king & next squares are not under attack
                            if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black)) {
                                add_move(move_list, encode_move(e1, c1, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                }
            } 
            // black pawn and castling moves
            else {
                // black pawn moves (moves that aren't capturing)
                if (board[square] == p) {
                    int target_square = square + 16;
                    // check if target square is on board
                    if (on_board(target_square) && is_empty_square(target_square)) {
                        // pawn promotions (make sure pawns are on 2nd rank)
                        if (rank_2(square)) {
                            add_move(move_list, encode_move(square, target_square, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(square, target_square, n, 0, 0, 0, 0));
                        } else {
                            // one square ahead pawn move
                            add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                            // two squares ahead pawn move (make sure pawns are on 7th rank)
                            if (rank_7(square) && is_empty_square(square + 32)) {
                                add_move(move_list, encode_move(square, target_square, 0, 0, 1, 0, 0));
                            }
                        }
                    }
                    // black pawn captures (using bishop offsets b/c they're the same as pawns)
                    for (int i = 0; i < 4; ++i) {
                        // init pawn offset
                        int pawn_offset = bishop_offsets[i];
                        
                        // black pawn offsets
                        if (pawn_offset > 0) {
                            int target_square = square + pawn_offset;
                            // check if target square is on board
                            if (on_board(target_square)) {
                                // capture pawn promotion
                                if (rank_2(square) && is_white_piece_square(target_square)) {
                                    add_move(move_list, encode_move(square, target_square, q, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, r, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, b, 1, 0, 0, 0));
                                    add_move(move_list, encode_move(square, target_square, n, 1, 0, 0, 0));
                                } else {
                                    // normal capture
                                    if (is_white_piece_square(target_square)) {
                                        add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                                    }
                                    // enpassant capture
                                    if (target_square == enpassant) {
                                        add_move(move_list, encode_move(square, target_square, 0, 1, 0, 1, 0));
                                    }
                                }
                            }
                        }
                    }
                }
                // black king castling
                if (board[square] == k) {
                    // if king side castling is available
                    if (castle & kc) {
                        // make sure there are empty squares between king & rook
                        if (board[f8] == e && board[g8] == e) {
                            // make sure king & next squares are not under attack
                            if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white)) {
                                add_move(move_list, encode_move(e8, g8, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                    // if queen side castling is available
                    if (castle & qc) {
                        // make sure there are empty squares between king & rook
                        if (board[b8] == e && board[c8] == e && board[d8] == e) {
                            // make sure king & next squares are not under attack
                            if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white)) {
                                add_move(move_list, encode_move(e8, c8, 0, 0, 0, 0, 1));
                            }
                        }
                    }
                }
            }
            // knight moves
            if (side == white ? board[square] == N : board[square] == n) {
                // loop over knight offsets
                for (int i = 0; i < 8; ++i) {
                    int target_square = square + knight_offsets[i];
                    int target_piece = board[target_square];
                    
                    if (on_board(target_square)) {
                        if (side == white ?
                           (is_empty_square(target_square) || is_black_piece(target_piece)) :
                           (is_empty_square(target_square) || is_white_piece(target_piece))) {
                            // test if we are capturing
                            if (target_piece != e) {
                                add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                            }
                            // moving to empty square
                            else {
                                add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                            }
                        }
                    }
                }
            }
            // king moves
            if (side == white ? board[square] == K : board[square] == k) {
                // loop over knight offsets
                for (int i = 0; i < 8; ++i) {
                    int target_square = square + king_offsets[i];
                    int target_piece = board[target_square];
                    
                    if (on_board(target_square)) {
                        if (side == white ?
                           (is_empty_square(target_square) || is_black_piece(target_piece)) :
                           (is_empty_square(target_square) || is_white_piece(target_piece))) {
                            // test if we are capturing
                            if (target_piece != e) {
                                add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                            }
                            // moving to empty square
                            else {
                                add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                            }
                        }
                    }
                }
            }
            // bishop & queen moves
            if (side == white ?
                ((board[square] == B) || (board[square] == Q)) :
                ((board[square] == b) || (board[square] == q))) {
                // loop over bishop & queen offsets
                for (int i = 0; i < 4; ++i) {
                    int target_square = square + bishop_offsets[i];
                    // loop over attack ray
                    while (on_board(target_square)) {
                        int target_piece = board[target_square];
                        // if hits own piece
                        if (side == white ? is_white_piece(target_piece) : is_black_piece(target_piece)) {
                            break;
                        }
                        // if hits enemy piece
                        if (side == white ? is_black_piece(target_piece) : is_white_piece(target_piece)) {
                            add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                            break;
                        }
                        // if empty square
                        if (is_empty_square(target_square)) {
                            add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                        }
                        // increment target square
                        target_square += bishop_offsets[i];
                    }
                }
            }
            // rook & queen moves
            if (side == white ?
                ((board[square] == R) || (board[square] == Q)) :
                ((board[square] == r) || (board[square] == q))) {
                // loop over rook & queen offsets
                for (int i = 0; i < 4; ++i) {
                    int target_square = square + rook_offsets[i];
                    // loop over attack ray
                    while (on_board(target_square)) {
                        int target_piece = board[target_square];
                        // if hits own piece
                        if (side == white ? is_white_piece(target_piece) : is_black_piece(target_piece)) {
                            break;
                        }
                        // if hits enemy piece
                        if (side == white ? is_black_piece(target_piece) : is_white_piece(target_piece)) {
                            add_move(move_list, encode_move(square, target_square, 0, 1, 0, 0, 0));
                            break;
                        }
                        // if empty square
                        if (is_empty_square(target_square)) {
                            add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                        }
                        // increment target square
                        target_square += rook_offsets[i];
                    }
                }
            }
        }
    }
}

// main driver
int main() {
    // initialize conversion arrays
    initialize_char_pieces();
    initialize_promoted_pieces();
    // create move_list instance
    moves move_list[1];

    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    parse_fen(fen);

    print_board();
    generate_moves(move_list);

    // loop over moves in a movelist
    for (int i = 0; i < move_list->count; ++i) {
        int move = move_list->moves[i];
        cout << square_to_coords[get_move_start(move)] << square_to_coords[get_move_target(move)] << endl;
    }
    cout << "Number of moves: " << move_list->count << endl;


    // cout << "move: " << square_to_coords[get_move_start(move)] << square_to_coords[get_move_target(move)] << endl;
    // cout << "promoted piece: " << promoted_pieces[get_promoted_piece(move)] << endl;
    // cout << "capture flag: " << get_move_capture(move) << endl;
    // cout << "double pawn push flag: " << get_move_pawn(move) << endl;
    // cout << "enpassant flag: " << get_move_enpassant(move) << endl;
    // cout << "castling flag: " << get_move_castling(move) << endl;
    return 0;
}