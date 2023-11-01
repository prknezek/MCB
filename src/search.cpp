#include "movegen.h"
#include "chess_utils.h"
#include "search.h"
#include "evaluate.h"
#include <vector>
#include <algorithm>

using std::vector;
using std::pair;
using std::make_pair;
using std::sort;

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
            if (is_white_piece(capture_piece))
                move_score = 10 * piece_value[0][capture_piece - 1] - piece_value[1][move_piece - 7];
            else if (is_black_piece(capture_piece))
                move_score = 10 * piece_value[1][capture_piece - 7] - piece_value[0][move_piece - 1];
        }

        // promoting a pawn is likely a good move
        if (get_promoted_piece(move) != e) {
            if (is_white_piece(get_promoted_piece(move))) {
                move_score += piece_value[0][get_promoted_piece(move) - 1];
            } else {
                move_score -= piece_value[1][get_promoted_piece(move) - 7];
            }
        }

        // penalize moving pieces to a square that's attacked by an opponent pawn
        if (is_square_attacked_pawn(get_move_target(move), side ^ 1)) {
            if (is_white_piece(move_piece)) {
                move_score -= piece_value[0][move_piece - 1];
            } else {
                move_score -= piece_value[1][move_piece - 7];
            }
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

// quiescence search
int quiescence(int alpha, int beta) {
    // base case we evaluate final board position
    int eval = evaluate();

    // fail-hard beta cutoff
    if (eval >= beta) {
        // node (move) fails high
        return beta;
    }

    // found a better move
    alpha = std::max(alpha, eval);

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
        if (!make_move(move_list->moves[i], only_captures)) {
            // go to next loop iteration
            continue;
        }

        int score = -quiescence(-beta, -alpha);
        
        // restore board position
        memcpy(board, board_copy, 512);
        side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
        memcpy(king_square, king_square_copy, 8);
    
        // fail-hard beta cutoff
        if (score >= beta) {
            // node (move) fails high
            return beta;
        }
        alpha = std::max(alpha, score);
    }
    // node (move) fails low
    return alpha;
}

// nega max function
int nega_max(int depth, int alpha, int beta) {
    // base case we evaluate final board position
    if (depth == 0) {
        nodes++;
        // run quiescence search to avoid horizon effect (capture of piece on next move)
        return quiescence(alpha, beta);
    }

    // best move so far
    int best_sofar;

    // old alpha value
    int old_alpha = alpha;

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

        nodes++;
        int score = -nega_max(depth - 1, -beta, -alpha);

        // restore board position
        memcpy(board, board_copy, 512);
        side = side_copy, enpassant = enpassant_copy, castle = castle_copy;
        memcpy(king_square, king_square_copy, 8);
    
        // fail-hard beta cutoff
        if (score >= beta) {
            // node (move) fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {
            // PV node (move)
            alpha = score;
            // if root move
            if (depth == DEPTH) {
                // associate best move with best score
                best_sofar = move_list->moves[i];
            }
        }

        // check for checkmate or stalemate
        if (legal_moves == 0) {
            if (in_check(side ^ 1)) {
                return -CHECKMATE;
            }
            return 0;
        }
    }
    
    if (old_alpha != alpha) {
        // init best move
        NEXT_MOVE = best_sofar;
    }

    // node (move) fails low
    return alpha;
}
