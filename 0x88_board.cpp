// headers
#include<stdio.h>
#include<iostream>
#include<limits>
#include<vector>
#include<utility>
#include<algorithm>
#include "chess_utils.h"
#include "movegen.h"

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
        
        // prioritze checks


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
        int legal_moves = move_list->count;
        if (!make_move(move_list->moves[i], all_moves)) {
            legal_moves--;
            continue;
        }

        // check for checkmate or stalemate
        if (legal_moves == 0) {
            if (in_check(side ^ 1)) {
                return -CHECKMATE;
            }
            return 0;
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
    char fen[] = "8/p3N2k/1p6/1P3Np1/P1B1P1Pn/8/5K2/3r3q b - - 3 48";
    parse_fen(tricky_position);
    print_board();
    
    // get best next move
    int start_time = get_time_ms();
    nega_max(DEPTH, -CHECKMATE, CHECKMATE);
    cout << "Nodes: " << nodes << endl;
    cout << "Time: " << get_time_ms() - start_time << "ms" << endl;

    return 0;
}