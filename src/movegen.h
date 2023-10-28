#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "chess_utils.h"

void old_generate_moves(moves *move_list);
void generate_moves(moves *move_list);
int make_move(int move, int capture_flag);

#endif