#ifndef TT_H
#define TT_H

extern unsigned int random_state;

extern uint64_t piece_keys[12][128];
extern uint64_t enpassant_keys[128];
extern uint64_t castling_keys[16];
extern uint64_t side_key;
extern uint64_t hash_key;

unsigned int rand32();
uint64_t rand64();
void init_random_keys();
uint64_t generate_zobrist_key();

#endif