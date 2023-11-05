#include "chess_utils.h"
#include <iostream>
#include <random>

// pseudo random number state
unsigned int random_state = 0xdeadbeef;

// random piece keys [piece][square]
uint64_t piece_keys[12][128];
// random enpassant keys [square]
uint64_t enpassant_keys[128];
// random castling keys
uint64_t castling_keys[16];
// random side key
uint64_t side_key;
// "almost" unique position ID aka Zobrist key
uint64_t hash_key;

// generate 32-bit pseudo random number
unsigned int rand32() {
    // xorshift algorithm
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;

    return random_state;
}

// generate 64-bit pseudo random number
uint64_t rand64() {
    // init 4 random 64-bit numbers
    uint64_t n1, n2, n3, n4;

    // init random numbers slicing 16 bits from MS1B side
    n1 = (uint64_t) (rand32()) & 0xFFFF;
    n2 = (uint64_t) (rand32()) & 0xFFFF;
    n3 = (uint64_t) (rand32()) & 0xFFFF;
    n4 = (uint64_t) (rand32()) & 0xFFFF;

    // return random 64-bit number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

// init random hash keys
void init_random_keys() {
    // update pseudo random number
    random_state = 0xdeadbeef;

    // loop over piece codes and squares
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 128; ++j) {
            // init random piece keys
            piece_keys[i][j] = rand64();
        }
    }

    // loop over all squares
    for (int i = 0; i < 128; ++i) {
        enpassant_keys[i] = rand64();
    }

    // loop over castling keys
    for (int i = 0; i < 16; ++i) {
        castling_keys[i] = rand64();
    }

    // init side key
    side_key = rand64();
}

// generate "almost" unique position ID aka Zobrist key
uint64_t generate_zobrist_key() {
    // final hash key
    uint64_t final_key = 0ULL;

    // loop over all squares
    for (int square = 0; square < 128; ++square) {
        // if square is empty, continue
        if (board[square] == e) {
            continue;
        }

        // only xor squares on board
        if (on_board(square)) {
            // get piece type : 0-5 for white pieces, 6-11 for black pieces
            // we subtract one to properly index into piece_keys
            int piece = board[square] - 1;

            // update final hash key
            final_key ^= piece_keys[piece][square];
        }
    }

    // if enpassant square is on board
    if (enpassant != no_sq) {
        // update final hash key with enpassant
        final_key ^= enpassant_keys[enpassant];
    }

    // update final hash key with castling rights
    final_key ^= castling_keys[castle];

    // update final hash key with side to move only if black
    if (side == black) {
        final_key ^= side_key;
    }

    // return final hash key
    return final_key;
}