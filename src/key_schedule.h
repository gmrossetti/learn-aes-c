#pragma once

#include <stdint.h>
#include <string.h>

#include "sbox.h"

static const uint8_t rc[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

void lrot_word(const uint8_t w[4], uint8_t dst[4])
{
    uint8_t temp = w[0];
    dst[0] = w[1];
    dst[1] = w[2];
    dst[2] = w[3];
    dst[3] = temp;
}

void sub_word(const uint8_t w[4], uint8_t dst[4])
{
    const uint8_t *sbox = aes_box();

    for (int i=0; i<4; i++)
    {
        dst[i] = sbox[w[i]];
    }
}

void rcon_word(const uint8_t w[4], int round, uint8_t dst[4])
{  
    dst[0] = w[0] ^ rc[round];
    dst[1] = w[1];
    dst[2] = w[2];
    dst[3] = w[3];
}

void xor_word(const uint8_t w0[4], const uint8_t w1[4], uint8_t dst[4])
{
    for (int i=0; i<4; i++)
    {
        dst[i] = w0[i] ^ w1[i];
    }
}

void gen_round_key(const uint8_t *k, size_t k_len, int round, uint8_t *dst)
{
    if (k == NULL || k_len == 0) {
        return;
    }

    const uint8_t w0[4] = 
    {
        k[0],
        k[1],
        k[2],
        k[3]
    };

    const uint8_t w1[4] = 
    {
        k[4],
        k[5],
        k[6],
        k[7]
    };

    const uint8_t w2[4] = 
    {
        k[8],
        k[9],
        k[10],
        k[11]
    };

    const uint8_t w3[4] = 
    {
        k[12],
        k[13],
        k[14],
        k[15]
    };

    uint8_t w3_rot[4];

    lrot_word(w3, w3_rot);

    uint8_t w3_sub[4];

    sub_word(w3_rot, w3_sub);

    uint8_t w3_rcon[4];
    
    rcon_word(w3_sub, round, w3_rcon);
    
    uint8_t w4[4];
    
    xor_word(w3_rcon, w0, w4);

    uint8_t w5[4];
    
    xor_word(w4, w1, w5);

    uint8_t w6[4];
    
    xor_word(w5, w2, w6);

    uint8_t w7[4];
    
    xor_word(w6, w3, w7);

    const size_t w_size = sizeof(w4);

    memcpy(&dst[0 * w_size], w4, w_size);
    memcpy(&dst[1 * w_size], w5, w_size);
    memcpy(&dst[2 * w_size], w6, w_size);
    memcpy(&dst[3 * w_size], w7, w_size);
}

void generate_round_keys(const uint8_t *k, const size_t k_len, uint8_t *dst, const size_t n_rows, const size_t n_cols)
{
    if (k == NULL || k_len == 0 || n_rows == 0 || n_cols == 0) {
        return; 
    }

    uint8_t rk[k_len];

    memcpy(rk, k, k_len);

    memcpy(&dst[0], &rk[0], n_cols);

    for (int i=1; i<11; i++)
    {
        gen_round_key(rk, 16, i-1, rk);
        memcpy(&dst[i * n_cols], &rk[0], n_cols);
    }
}