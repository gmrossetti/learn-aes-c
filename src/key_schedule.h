#pragma once

#include <stdint.h>
#include <string.h>

static const uint8_t rc[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

void lrot_word(const uint8_t w[4], uint8_t dst[4]);

void sub_word(const uint8_t w[4], uint8_t dst[4]);

void rcon_word(const uint8_t w[4], int round, uint8_t dst[4]);

void xor_word(const uint8_t w0[4], const uint8_t w1[4], uint8_t dst[4]);

void gen_round_key(const uint8_t *k, size_t k_len, int round, uint8_t *dst);

void generate_round_keys(const uint8_t *k, const size_t k_len, uint8_t *dst, const size_t n_rows, const size_t n_cols);