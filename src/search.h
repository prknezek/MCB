#ifndef SEARCH_H
#define SEARCH_H

#include "chess_utils.h"

void order_moves(moves *move_list);
int nega_max(int depth, int alpha, int beta);

#endif