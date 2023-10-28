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
        // at depth 0 evaluate all capturing moves until there aren't any
        // this makes it so positions don't leave pieces hanging 
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
