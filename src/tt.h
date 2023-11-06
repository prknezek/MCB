#ifndef TT_H
#define TT_H

extern unsigned int random_state;

extern uint64_t piece_keys[12][128];
extern uint64_t enpassant_keys[128];
extern uint64_t castling_keys[16];
extern uint64_t side_key;
extern uint64_t hash_key;

// hash table size in MB (4MB)
#define hash_size 0x100000 * 4

// transposition table hash flags
#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta  2

// no hash empty found constant
#define no_hash_entry 100000

unsigned int rand32();
uint64_t rand64();
void init_random_keys();
uint64_t generate_zobrist_key();

void clear_tt();
int read_hash_entry(int depth, int alpha, int beta);
void write_hash_entry(int depth, int eval, int flag);

#endif