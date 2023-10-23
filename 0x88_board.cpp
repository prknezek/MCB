// headers
#include<stdio.h>
#include<iostream>
#include<limits>
#include<vector>
#include<utility>
#include<algorithm>
#include "chess_utils.h"

using std::vector;
using std::pair;
using std::numeric_limits;
using std::sort;
using std::make_pair;

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
                                add_move(move_list, encode_move(square, square - 32, 0, 0, 1, 0, 0));
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
                                add_move(move_list, encode_move(square, square + 32, 0, 0, 1, 0, 0));
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

// score the board based on material
int evaluate() {
    int score = 0;
    // loop over all board squares
    for (int square = 0; square < 128; ++square) {
        // if square is on board
        if (on_board(square)) {
            // if square is not empty
            if (board[square] != e) {
                if (is_white_piece(board[square])) {
                    // add piece value to score
                    score += piece_value[board[square]];
                } else {
                    // subtract piece value from score
                    score -= piece_value[board[square]];
                }
            }
        }
    }
    return score * (side == white ? 1 : -1);
}

// order moves for a more efficient negamax function 
void order_moves(moves *move_list) {
    // create a vector of pairs to store the move and its score
    vector<pair<int, int>> move_scores;

    for (int i = 0; i < move_list->count; ++i) {
        int move = move_list->moves[i];

        int move_score = 0;
        int move_piece = board[get_move_start(move)];
        int capture_piece = board[get_move_target(move)];
        
        // prioritize capturing opponent's most valuable piece with out least valuable piece
        if (capture_piece != e) {
            move_score = 10 * piece_value[capture_piece] - piece_value[move_piece];
        }

        // promoting a pawn is likely a good move
        if (get_promoted_piece(move) != e) {
            move_score += piece_value[get_promoted_piece(move)];
        }

        // penalize moving pieces to a square that's attacked by an opponent pawn
        if (is_square_attacked_pawn(get_move_target(move), side ^ 1)) {
            move_score -= piece_value[move_piece];
        }

        // add the move and its score to the vector
        move_scores.push_back(make_pair(move, move_score));
    }

    // sort the vector based on the move score (in descending order)
    sort(move_scores.begin(), move_scores.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    });

    // update the move_list with the sorted moves
    for (int i = 0; i < move_list->count; ++i) {
        move_list->moves[i] = move_scores[i].first;
    }
}

int CHECKMATE = numeric_limits<int>::max();
int DEPTH = 4;
int NEXT_MOVE = 0;
int nodes = 0;

// nega max function
int nega_max(int depth, int alpha, int beta) {
    // base case we evaluate final board position
    if (depth == 0) {
        nodes++;
        return evaluate();
    }

    int max = -CHECKMATE;
    moves move_list[1];
    // generate all possible moves at current board position
    generate_moves(move_list);
    // order moves based on heuristics
    order_moves(move_list);
    // go through all possible moves
    for (int i = 0; i < move_list->count; i++) {
        // create board state copy variables
        int board_copy[128], king_square_copy[2];
        int side_copy, enpassant_copy, castle_copy;

        // copy board state
        memcpy(board_copy, board, 512);
        side_copy = side, enpassant_copy = enpassant, castle_copy = castle;
        memcpy(king_square_copy, king_square, 8);

        // if we make an illegal move skip it
        if (!make_move(move_list->moves[i], all_moves)) {
            continue;
        }

        nodes++;
        int score = -1 * nega_max(depth - 1, -beta, -alpha);
        
        if (score > max) {
            max = score;
            if (depth == DEPTH) {
                cout << "\nNext move: ";
                print_move(move_list->moves[i]);
                cout << "Score: " << score << endl;
                NEXT_MOVE = move_list->moves[i];
            }
        }

        // alpha beta pruning
        if (max > alpha) {
            alpha = max;
        }

        // restore board position
        memcpy(board, board_copy, 512);
        side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
        memcpy(king_square, king_square_copy, 8);
    
        if (alpha >= beta) {
            return alpha;
        }
    }
    return max;
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

    // moves move_list[1];
    // generate_moves(move_list);
    // order_moves(move_list);
    
    int start_time = get_time_ms();
    nega_max(DEPTH, -CHECKMATE, CHECKMATE);
    cout << "Nodes: " << nodes << endl;
    cout << "Time: " << get_time_ms() - start_time << "ms" << endl;
    //perft_test(4);

    return 0;
}