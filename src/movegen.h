#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "chess_utils.h"

void generate_moves(moves *move_list);
void new_generate_moves(moves *move_list);
int make_move(int move, int capture_flag);

#endif