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

// half move counter
int ply;

// max ply that we can reach within a search
#define max_ply 64
// null move pruning constant
#define R 2

// killer moves [id][ply]
int killer_moves[2][max_ply];
// history moves [piece][square]
int history_moves[12][64];

// PV length [ply]
int pv_length[max_ply];
// PV table [ply][ply]
int pv_table[max_ply][max_ply];
// follow PV & score PV move
int follow_pv, score_pv;

// late move reduction variables
int full_depth_moves = 4;
int LMR_limit = 3;

// enable PV move scoring
void enable_pv_scoring(moves *move_list) {
    // disable following PV line
    follow_pv = 0;
    
    for (int i = 0; i < move_list->count; ++i) {
        // make sure we hit PV move
        if (pv_table[0][ply] == move_list->moves[i]) {
            // enable move scoring
            score_pv = 1;
            // enable following PV line
            follow_pv = 1;
        }
    }
}

/*
    ========================
         Move ordering
    ========================
    1. PV move
    2. Captures in MVV/LVA
    3. 1st killer move
    4. 2nd killer move
    5. History moves
    6. Pawn promotions
    7. Unsorted moves
*/

// return the score of a move to be used for move ordering
int score_move(int move) {
    if (score_pv) {
        if (pv_table[0][ply] == move) {
            score_pv = 0;
            return 20000;
        }
    }

    int move_piece = board[get_move_start(move)];
    int capture_piece = board[get_move_target(move)];

    if (get_move_capture(move)) {

        if (is_white_piece(capture_piece))
           return 10 * piece_value[0][capture_piece - 1] - piece_value[1][move_piece - 7] + 10000;
        else if (is_black_piece(capture_piece))
            return 10 * piece_value[1][capture_piece - 7] - piece_value[0][move_piece - 1] + 10000;
    } else {
        if (killer_moves[0][ply] == move) {
            return 9000;
        }
        // score 2nd killer move
        else if (killer_moves[1][ply] == move) {
            return 8000;
        }
        // score history move
        else {
           return history_moves[move_piece - 1][get_move_target(move)];
        }
    }
}

// order moves for a more efficient negamax function 
void order_moves(moves *move_list) {
    // create a vector of pairs to store the move and its score
    vector<pair<int, int>> move_scores;

    for (int i = 0; i < move_list->count; ++i) {
        int move = move_list->moves[i];

        int move_score = score_move(move);

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
    // increment nodes count
    nodes++;

    if (ply > max_ply - 1) {
        return evaluate();
    }

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

    // init legal moves for detecting checkmate or stalemate
    int legal_moves = 0;

    // go through all possible moves
    for (int i = 0; i < move_list->count; i++) {
        // copy board state
        copy_board();

        // increment ply
        ply++;

        // if we make an illegal move skip it
        if (!make_move(move_list->moves[i], only_captures)) {
            // decrement ply
            ply--;
            // go to next loop iteration
            continue;
        }
        legal_moves++;

        // score current move
        int score = -quiescence(-beta, -alpha);
        
        // decrement ply
        ply--;

        // restore board position
        restore_board();

        // found a better move
        if (score > alpha) {
            // PV node
            alpha = score;
            // fail-hard beta cutoff
            if (score >= beta) {
                // node (move) fails high
                return beta;
            }
        }
    }

    // node (move) fails low
    return alpha;
}

// nega max function
int nega_max(int depth, int alpha, int beta) {
    // init score
    int score = 0;

    // define hash flag
    int hash_flag = hash_flag_alpha;

    // read hash entry
    if (ply && (score = read_hash_entry(depth, alpha, beta)) != no_hash_entry) {
        // if the move has already been searched (has a value)
        return score;
    }

    // init PV length
    pv_length[ply] = ply;

    // base case we evaluate final board position
    if (depth == 0) {
        // run quiescence search to avoid horizon effect (capture of piece on next move)
        return quiescence(alpha, beta);
    }
    // we are too deep, so we will overflow arrays relying on max ply constant
    if (ply > max_ply - 1) {
        return evaluate();
    }

    // increment nodes count
    nodes++;

    // increase search depth if king is in check
    if (in_check(side^1)) {
        depth++;
    }
    // init legal moves for detecting checkmate or stalemate
    int legal_moves = 0;

    // Null Move Pruning
    if (depth >= 3 && !in_check(side^1) && ply) {
        // copy board state
        copy_board();

        // increment ply
        ply++;

        // hash enpassant if available
        if (enpassant != no_sq) {
            hash_key ^= enpassant_keys[enpassant];
        }

        // reset enpassant capture square
        enpassant = no_sq;
        // switch side to give opponent an extra (null) move
        side ^= 1;

        // hash the side
        hash_key ^= side_key;

        // search moves with reduced depth to find beta cutoffs
        score = -nega_max(depth - 1 - R, -beta, -beta + 1);

        // decrement ply
        ply--;

        // restore board state
        restore_board();

        // fail hard beta cutoff
        if (score >= beta)
            // node fails high
            return beta;
    }

    moves move_list[1];
    // generate all possible moves at current board position
    generate_moves(move_list);

    // if we are following the PV line
    if (follow_pv) {
        // enable PV move scoring
        enable_pv_scoring(move_list);
    }

    // order moves based on heuristics
    order_moves(move_list);

    // number of moves searched in a move list
    int moves_searched = 0;

    int move = 0;

    // go through all possible moves
    for (int i = 0; i < move_list->count; i++) {
        // copy board state
        copy_board();

        // increment ply
        ply++;

        // MAKE THE MOVE
        // if we make an illegal move skip it
        if (make_move(move_list->moves[i], all_moves) == 0) {
            // decrement ply
            ply--;
            // skip to the next move
            continue;
        }
        move = move_list->moves[i];
        legal_moves++;

        // Full depth search for first move
        if (moves_searched == 0) {
            // normal nega_max search with full window
            score = -nega_max(depth - 1, -beta, -alpha);
        } 
        // Late Move Reduction
        else {
            // conditions to consider late move reduction (LMR)
            /* we reduce moves (LMR) if:
                    1. we're inside the LMR window
                    2. the move does not give a check
                    3. the move is not a capture
                    4. the move is not a promotion
            */
            if (moves_searched >= full_depth_moves &&
                depth >= LMR_limit &&
                in_check(side^1) == 0 &&
                get_move_capture(move_list->moves[i]) == 0 &&
                get_promoted_piece(move_list->moves[i]) == 0) {
                // search move with reduced depth
                score = -nega_max(depth - 2, -alpha - 1, -alpha);
            } else {
                // change score to ensure that full-depth search is done
                score = alpha + 1;
            }
            // Principle Variation Search PVS
            if (score > alpha) {
                // normal nega_max search with narrowed window
                // cout << "PRINCIPLE VARIATION SEARCH ";
                // cout << get_move_string(move_list->moves[i]) << endl;
                score = -nega_max(depth - 1, -alpha - 1, -alpha);
                // cout << "SCORE: " << score << endl;
                // if LMR fails re-search at full depth and full window
                if ((score > alpha) && (score < beta)) {
                    // cout << "LMR Failed: " << get_move_string(move_list->moves[i]);
                    // printf("Score: %d, Alpha: %d, Beta: %d\n\n", score, alpha, beta);
                    score = -nega_max(depth - 1, -beta, -alpha);
                }
            }
        }

        // decrement ply
        ply--;

        // restore board state
        restore_board();

        // increment the number of moves searched so far
        moves_searched++;

        // found a better move
        if (score > alpha) {
            // switch hash flag from storing score for fail-low to PV node
            hash_flag = hash_flag_exact;
            // on quiet moves
            if (get_move_capture(move_list->moves[i]) == 0) {
                int piece = board[get_move_start(move_list->moves[i])];
                // store history moves
                history_moves[piece - 1][get_move_target(move_list->moves[i])] += depth * depth;
            }
            // PV node (move)
            alpha = score;
            // cout << "SETTING ALPHA TO SCORE BECAUSE BETTER MOVE: ";
            get_move_string(move_list->moves[i]);

            // write PV move to PV table
            pv_table[ply][ply] = move_list->moves[i];
            // loop over the next ply
            for (int j = ply + 1; j < pv_length[ply + 1]; j++) {
                // copy move from deeper ply into a current ply's line
                pv_table[ply][j] = pv_table[ply + 1][j];
            }
            // adjust PV length
            pv_length[ply] = pv_length[ply + 1];

            // fail-hard beta cutoff
            if (score >= beta) {
                // write hash entry
                write_hash_entry(depth, beta, hash_flag_beta);
                // on quiet moves
                if (get_move_capture(move_list->moves[i]) == 0) {
                    // store killer moves
                    // we store second killer first because it was killer move of previous iteration
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list->moves[i];
                }
                // node (move) fails high
                // cout << "RETURNING BETA: " << beta << endl;
                return beta;
            }
        }
    }

    // check for checkmate or stalemate
    if (legal_moves == 0) {
        if (in_check(side ^ 1)) {
            // cout << "CHECKMATE*************** RETURNING: " << -CHECKMATE + ply << endl;
            return -CHECKMATE + ply;
        } else {
            return 0;
        }
    }

    // write hash entry
    write_hash_entry(depth, alpha, hash_flag);
    // node (move) fails low
    return alpha;
}

// search position for the best move
void search(int depth) {
    // init score variable
    int score = 0;
    // reset nodes and flags
    nodes = 0;
    follow_pv = 0;
    score_pv = 0;

    // clear helper data structures for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    int alpha = -50000;
    int beta = 50000;

    // iterative deepening
    int current_depth = 1;
    while (current_depth <= depth) {
        // enable follow PV flag
        follow_pv = 1;
        // get best next move at current depth
        score = nega_max(current_depth, alpha, beta);
        // cout << "SCORE: " << score << endl;

        // Aspiration Window
        // we fell outside the window so try again with full-width window
        if ((score <= -50000) || (score >= 50000)) {
            alpha = -50000;
            beta = 50000;
            current_depth--;
            continue;
        }
        // set up the window for the next iteration
        alpha = score - 50;
        beta = score + 50;

        cout << "\nDepth: " << current_depth << "   ";
        cout << "Score: " << score << "   ";
        cout << "Nodes: " << nodes << endl;

        cout << "PV Line " << pv_length[0] << ": ";
        for (int i = 0; i < pv_length[0]; ++i) {
            int move = pv_table[0][i];
            cout << square_to_coords[get_move_start(move)] << square_to_coords[get_move_target(move)] << promoted_pieces[get_promoted_piece(move)];
            cout << " ";
        }
        cout << endl;

        // increment current depth
        current_depth++;
    }
    
    NEXT_MOVE = pv_table[0][0];
    cout << "Best Move: " << get_move_string(NEXT_MOVE) << endl;
}