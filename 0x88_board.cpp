// headers
#include <stdio.h>
#include <iostream>
#include "chess_utils.h"

// FEN debug positions
char start_position[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char tricky_position[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

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

// generate all pawn moves
void generate_pawn_moves(moves *move_list, int square) {
    // detect if we're generating white or black pawn moves
    if (side == white ? board[square] == P : board[square] == p) {
        int target_square = square + (side == white ? -16 : 16);

        // quiet pawn moves
        if (on_board(target_square) && is_empty_square(target_square)) {
            // pawn promotions
            if (side == white ? rank_7(square) : rank_2(square)) {
                add_move(move_list, encode_move(square, target_square, (side == white ? Q : q), 0, 0, 0, 0));
                add_move(move_list, encode_move(square, target_square, (side == white ? R : r), 0, 0, 0, 0));
                add_move(move_list, encode_move(square, target_square, (side == white ? B : b), 0, 0, 0, 0));
                add_move(move_list, encode_move(square, target_square, (side == white ? N : n), 0, 0, 0, 0));
            } else {
                // one square pawn move
                add_move(move_list, encode_move(square, target_square, 0, 0, 0, 0, 0));
                // two square pawn move
                int pawn_move_offset = (side == white ? -32 : 32);
                if ((side == white ? rank_2(square) : rank_7(square)) && is_empty_square(square + pawn_move_offset)) {
                    add_move(move_list, encode_move(square, square + pawn_move_offset, 0, 0, 1, 0, 0));
                }
            }
        }
        // pawn captures (using bishop offsets b/c they're the same as pawns)
        for (int i = 0; i < 4; ++i) {
            // init pawn offset
            int pawn_offset = bishop_offsets[i];
            
            if ((side == white ? pawn_offset < 0 : pawn_offset > 0)) {
                int target_square = square + pawn_offset;
                // check if target square is on board
                if (on_board(target_square)) {
                    // capture pawn promotion
                    bool capture_side = (side == white ? is_black_piece_square(target_square) : is_white_piece_square(target_square));
                    if ((side == white ? rank_7(square) : rank_2(square)) && capture_side) {
                        add_move(move_list, encode_move(square, target_square, (side == white ? Q : q), 1, 0, 0, 0));
                        add_move(move_list, encode_move(square, target_square, (side == white ? R : r), 1, 0, 0, 0));
                        add_move(move_list, encode_move(square, target_square, (side == white ? B : b), 1, 0, 0, 0));
                        add_move(move_list, encode_move(square, target_square, (side == white ? N : n), 1, 0, 0, 0));
                    } else {
                        // normal capture
                        if (capture_side) {
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
}

// generate all castle moves
void generate_castle_moves(moves *move_list, int square) {
    // detect if we're generating white or black castling moves
    if (side == white ? board[square] == K : board[square] == k) {
        // king side castling
        if (side == white ? castle & KC : castle & kc) {
            // get castling squares
            int squares[3] = {e8, f8, g8};
            if (side == white) {
                squares[0] = e1;
                squares[1] = f1;
                squares[2] = g1;
            }
            // make sure there are empty squares between king & rook
            if ((board[squares[1]] == e && board[squares[2]] == e)) {
                int side_attacked = (side == white ? black : white);
                if (!is_square_attacked(squares[0], side_attacked) && !is_square_attacked(squares[1], side_attacked)) {
                    add_move(move_list, encode_move(squares[0], squares[2], 0, 0, 0, 0, 1));
                }
            }
        }
        // if queen side castling is available
        if (side == white ? castle & QC : castle & qc) {
            // get castling squares
            int squares[4] = {b8, c8, d8, e8};
            if (side == white) {
                squares[0] = b1;
                squares[1] = c1;
                squares[2] = d1;
                squares[3] = e1;
            }
            // make sure there are empty squares between king & rook
            if (board[squares[0]] == e && board[squares[1]] == e && board[squares[2]] == e) {
                // make sure king & next squares are not under attack
                if (!is_square_attacked(squares[3], black) && !is_square_attacked(squares[2], black)) {
                    add_move(move_list, encode_move(squares[3], squares[1], 0, 0, 0, 0, 1));
                }
            }
        }
    }
}

// generates all knight and king moves
void generate_NK_moves(moves *move_list, int square, int white_piece, int black_piece) {
    if (side == white ? board[square] == white_piece : board[square] == black_piece) {
        // loop over knight offsets
        for (int i = 0; i < 8; ++i) {
            int target_square = square + (white_piece == N ? knight_offsets[i] : king_offsets[i]);
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
}

void generate_QBR_moves(moves *move_list, int square, int white_piece, int black_piece) {
    // bishop & queen moves
    if (side == white ?
        ((board[square] == white_piece) || (board[square] == Q)) :
        ((board[square] == black_piece) || (board[square] == q))) {
        // loop over bishop & queen offsets
        for (int i = 0; i < 4; ++i) {
            int target_square = square + (white_piece == B ? bishop_offsets[i] : rook_offsets[i]);
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
                target_square += (white_piece == B ? bishop_offsets[i] : rook_offsets[i]);
            }
        }
    }
}

// move generator
void generate_moves(moves *move_list) {
    // reset move count
    move_list->count = 0;
    // loop over all board squares
    for (int square = 0; square < 128; ++square) {
        if (on_board(square)) {
            generate_pawn_moves(move_list, square);
            generate_castle_moves(move_list, square);
            generate_NK_moves(move_list, square, N, n);
            generate_NK_moves(move_list, square, K, k);
            generate_QBR_moves(move_list, square, B, b);
            generate_QBR_moves(move_list, square, R, r);
        }
    }
}

int make_move(int move, int capture_flag) {
    // make quiet move
    if (capture_flag == all_moves) {
        // create board state copy variables
        int board_copy[128], king_square_copy[2];
        int side_copy, enpassant_copy, castle_copy;

        // copy board state
        memcpy(board_copy, board, 512);
        side_copy = side, enpassant_copy = enpassant, castle_copy = castle;
        memcpy(king_square_copy, king_square, 8);

        // decode move
        int from_square = get_move_start(move);
        int target_square = get_move_target(move);
        int promoted_piece = get_promoted_piece(move);
        int enpassant_flag = get_move_enpassant(move);
        int double_pawn_move_flag = get_move_pawn(move);
        int castle_flag = get_move_castling(move);

        // move piece
        board[target_square] = board[from_square];
        board[from_square] = e;

        // pawn promotion
        if (promoted_piece != e) {
            board[target_square] = promoted_piece;
        }

        // enpassant capture
        if (enpassant_flag) {
            side == white ? 
                (board[target_square + 16] = e) : 
                (board[target_square - 16] = e);
        }

        // reset enpassant square
        enpassant = no_sq;

        // double pawn push
        if (double_pawn_move_flag) {
            side == white ? 
                (enpassant = target_square + 16) :
                (enpassant = target_square - 16);
        }

        // castle move
        if (castle_flag) {
            // switch target square
            switch (target_square) {
                // white king side castling
                case g1:
                    board[f1] = board[h1];
                    board[h1] = e;
                    break;
                // white queen side castling
                case c1:
                    board[d1] = board[a1];
                    board[a1] = e;
                    break;
                // black king side castling
                case g8:
                    board[f8] = board[h8];
                    board[h8] = e;
                    break;
                // black queen side castling
                case c8:
                    board[d8] = board[a8];
                    board[a8] = e;
                    break; 
            }
        }

        // update king square
        if (board[target_square] == K || board[target_square] == k) {
            king_square[side] = target_square;
        }

        // update castling rights
        castle &= castling_rights[from_square];
        castle &= castling_rights[target_square];

        // change side
        side ^= 1;

        // take move back if king is in check
        if (is_square_attacked(king_square[side ^ 1], side)) {
            // restore board position
            memcpy(board, board_copy, 512);
            side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
            memcpy(king_square, king_square_copy, 8);
            // illegal move
            return 0;
        }
        // legal move
        return 1;
    } else {
        // capture move

        // if move is a capture
        if (get_move_capture(move)) {
            // make capture move
            make_move(move, all_moves);
        } else {
            // move is not a capture
            return 0;
        }
    }
}

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
        // create board state copy variables
        int board_copy[128], king_square_copy[2];
        int side_copy, enpassant_copy, castle_copy;

        // copy board state
        memcpy(board_copy, board, 512);
        side_copy = side, enpassant_copy = enpassant, castle_copy = castle;
        memcpy(king_square_copy, king_square, 8);

        // if we make an illegal move skip it
        if(!make_move(move_list->moves[i], all_moves)) {
            continue;
        }
        // make recursive call
        nodes += perft_driver(depth - 1);

        // restore board position
        memcpy(board, board_copy, 512);
        side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
        memcpy(king_square, king_square_copy, 8);
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

// main driver
int main() {
    // initialize conversion arrays
    initialize_char_pieces();
    initialize_promoted_pieces();

    // parse fen string
    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    parse_fen(fen);
    
    print_board();

    perft_test(4);

    return 0;
}