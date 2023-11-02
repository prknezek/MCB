#ifndef SEARCH_H
#define SEARCH_H

#include "chess_utils.h"


extern int ply;
extern int killer_moves[2][64];
extern int history_moves[12][64];
extern int pv_length[64];
extern int pv_table[64][64];
extern int full_depth_moves;
extern int LMR_limit;

void copy_board_state();
void restore_board_state();

void order_moves(moves *move_list);
int nega_max(int depth, int alpha, int beta);
void search(int depth);

#endif