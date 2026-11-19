#pragma once

#include <stdint.h>
#include <string.h>

enum AES_ROUND_TYPE {
    AES_ROUND_FIRST,
    AES_ROUND_INTERMEDIATE,
    AES_ROUND_LAST,
};

uint8_t gf_mul(uint8_t a, uint8_t b);

void add_round_key(uint8_t status[16], const uint8_t k[16]);

// TODO: implement inverse
void sub_bytes(uint8_t status[16], int inverse);

void lshift_word(uint8_t w[4], unsigned offset);

void shift_rows(uint8_t status[16]);

void mix_cols(uint8_t status[16]);

void round_encrypt(uint8_t status[], const uint8_t k[16], const enum AES_ROUND_TYPE round_type);

void encrypt(const uint8_t k[16], const size_t k_len, const uint8_t message[16], uint8_t ciphertext[16]);