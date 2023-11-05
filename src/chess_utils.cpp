#include "chess_utils.h"
#include "tt.h"

#include <chrono>
#include <string.h>

// returns if the given square is attacked by a user-defined side
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

    return 0;
}

// returns if king is in check by user-defined side (modified square attack function)
int in_check(int side) {
    return is_square_attacked(king_square[side ^ 1], side);
}

// returns if the given square is attacked by a pawn
int is_square_attacked_pawn(int square, int side) {
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
    return 0;
}

// print move list
void print_move_list(moves *move_list) {
    cout << "\n Moves:   Capture  Double  Enpassant  Castle\n" << endl;
    // loop over moves in a movelist
    for (int i = 0; i < move_list->count; ++i) {
        int move = move_list->moves[i];
        cout << " " << square_to_coords[get_move_start(move)] << square_to_coords[get_move_target(move)];
        cout << (get_promoted_piece(move) ? promoted_pieces[get_promoted_piece(move)] : ' ');
        cout << "    " << get_move_capture(move)
             << "        " << get_move_pawn(move)
             << "       " << get_move_enpassant(move)
             << "          " << get_move_castling(move) << endl;
    }
    cout << "\n Number of moves: " << move_list->count << endl;
}

// print move
void print_move(int move) {
    cout << "\n Moves:   Capture  Double  Enpassant  Castle\n" << endl;
    cout << " " << square_to_coords[get_move_start(move)] << square_to_coords[get_move_target(move)];
    cout << (get_promoted_piece(move) ? promoted_pieces[get_promoted_piece(move)] : ' ');
    cout << "    " << get_move_capture(move)
            << "        " << get_move_pawn(move)
            << "       " << get_move_enpassant(move)
            << "          " << get_move_castling(move) << endl;
}

// prints a board representation of attacked squares
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
                cout << ascii_pieces[board[square]] << "  ";
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
    cout << " King square: " << square_to_coords[king_square[side]] << endl;
    // print position ID aka Zobrist key
    printf(" \nZobrist key: %lx\n", generate_zobrist_key());
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
                    // set up kings' square
                    if (*fen == 'K') {
                        king_square[white] = square;
                    } else if (*fen == 'k') {
                        king_square[black] = square;
                    }
                    
                    // set the piece on the board
                    board[square] = char_pieces[*fen];

                    // increment FEN pointer to next char
                    *fen++;
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

    // create zobrist hash for the posiiton
    hash_key = generate_zobrist_key();
}

// determines if a square is on the board or not
bool on_board(int square) {
    return (!(square & 0x88));
}

// determines if a square is empty or not
bool is_empty_square(int square) {
    return (board[square] == e);
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

// get time in milliseconds
int get_time_ms() {
    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// get move string for UCI commands
string get_move_string(int move) {
    char promoted_piece = (get_promoted_piece(move) ? promoted_pieces[get_promoted_piece(move)] : ' ');
    if (promoted_piece == ' ') {
        return square_to_coords[get_move_start(move)] + square_to_coords[get_move_target(move)] + "\n";
    }
    return square_to_coords[get_move_start(move)] + square_to_coords[get_move_target(move)] + promoted_piece + "\n";
}

// print out piece counts
void print_piece_counts() {
    for (int i = 0; i < 12; i++) {
        if (i < 6) {
            cout << ascii_pieces[i + 1] << ": " << piece_count[white][i] << endl;
        } else {
            cout << ascii_pieces[i + 1] << ": " << piece_count[black][i - 6] << endl;
        }
    }
}

// add piece to piece count
void add_piece_count(int piece) {
    if (is_white_piece(piece)) {
        piece_count[white][piece - 1]++;
    } else {
        piece_count[black][piece - 7]++;
    }
}

// remove piece from piece count
void subtract_piece_count(int piece) {
    if (is_white_piece(piece)) {
        piece_count[white][piece - 1]--;
    } else {
        piece_count[black][piece - 7]--;
    }
}

// fill every index of piece_squares array with no_sq
void fill_piece_squares() {
    std::fill(&piece_squares[0][0][0], 
              &piece_squares[0][0][0] + sizeof(piece_squares) / sizeof(piece_squares[0][0][0]),
              no_sq);
}

// print piece squares
void print_piece_squares() {
    // iterate over piece_squares array
    for (int i = 0; i < 12; i++) {
        if (i < 6) {
            cout << ascii_pieces[i + 1] << ": ";
            // iterate over squares for each white piece
            for (int j = 0; j < 10; ++j) {
                if (piece_squares[white][i][j] != no_sq)
                    cout << square_to_coords[piece_squares[white][i][j]] << " ";
            }
            cout << endl;
        } else {
            cout << ascii_pieces[i + 1] << ": ";
            // iterate over squares for each black piece
            for (int j = 0; j < 10; ++j) {
                if (piece_squares[black][i - 6][j] != no_sq)
                    cout << square_to_coords[piece_squares[black][i - 6][j]] << " ";
            }
            cout << endl;
        }
    }
}

// adds a piece's square to the piece_squares array
void add_piece_square(int square) {
    int piece = board[square];
    if (is_white_piece(piece)) {
        int *indices = piece_squares[white][piece - 1];
        // find first spot that is no_sq in piece_squares
        for (int i = 0; i < 10; ++i) {
            if (indices[i] == no_sq) {
                indices[i] = square;
                break;
            }
        }
    } else {
        int *indices = piece_squares[black][piece - 7];
        // find first spot that is no_sq in piece_squares
        for (int i = 0; i < 10; ++i) {
            if (indices[i] == no_sq) {
                indices[i] = square;
                break;
            }
        }
    }
}

// update a piece's old square to a new square
// EX: update_piece_square(e2, e4) updates the square of the piece on e2 to e4
// DOES NOT MOVE THE PIECE ON THE BOARD
void update_piece_square(int piece, int old_square, int new_square) {
    // get piece on old square
    if (is_white_piece(piece)) {
        int *indices = piece_squares[white][piece - 1];
        // find old square in indices and replace with new square
        for (int i = 0; i < 10; ++i) {
            if (indices[i] == old_square) {
                indices[i] = new_square;
                break;
            }
        }
    } else {
        int *indices = piece_squares[black][piece - 7];
        // find old square in indices and replace with new square
        for (int i = 0; i < 10; ++i) {
            if (indices[i] == old_square) {
                indices[i] = new_square;
                break;
            }
        }
    }
}