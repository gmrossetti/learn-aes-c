#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "sbox.h"
#include "key_schedule.h"

uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t result = 0;

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            result ^= a;
        }

        uint8_t high_bit = a & 0x80;
        a <<= 1;

        if (high_bit) {
            a ^= 0x1B;
        }

        b >>= 1;
    }

    return result;
}

enum AES_ROUND_TYPE {
    AES_ROUND_FIRST,
    AES_ROUND_INTERMEDIATE,
    AES_ROUND_LAST,
};

void add_round_key(uint8_t status[16], const uint8_t k[16])
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            const int idx = i * 4 + j;
            uint8_t pre =  status[idx];
            status[idx] = status[idx] ^ k[j * 4 + i]; // col major access
        }
    }
}

// TODO: implement inverse
void sub_bytes(uint8_t status[16], int inverse)
{
    if (inverse) return;

    const uint8_t *sbox = aes_box();

    // Substitution Bytes
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            const int idx = i * 4 + j;
            status[idx] = sbox[status[idx]];
        }
    }
}

void lshift_word(uint8_t w[4], unsigned offset)
{
    offset %= 4; // 0..3

    if (offset == 0) return;

    uint8_t tmp[4];
    for (unsigned i = 0; i < 4; i++)
        tmp[i] = w[(i + offset) % 4];

    memcpy(w, tmp, 4);
}

void shift_rows(uint8_t status[16])
{
    // Shift Rows

    // Row 0: nothing to do
    // Row 1..3:
    for (unsigned r=1; r<=3; r++)
    {
        uint8_t *row = &status[r * 4];
        lshift_word(row, r);
    }
}

void mix_cols(uint8_t status[16])
{
    // Mix Column
    static const uint8_t mix_col_mul_matrix[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };

    uint8_t new_status[4 * 4];
    for (unsigned int i = 0; i < 4; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            uint8_t acc = 0;

            for (unsigned int k = 0; k < 4; k++)
            {
                uint8_t res = gf_mul(mix_col_mul_matrix[i][k], status[k * 4 + j]);
                acc ^= res;
            }

            new_status[i * 4 + j] = acc;
        }
    }

    // updates original state matrix with new mixed values
    for (unsigned int i = 0; i < 4; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            const int idx = i * 4 + j;
            status[idx] = new_status[idx];
        }
    }
}

void round_encrypt(uint8_t status[], const uint8_t k[16], const enum AES_ROUND_TYPE round_type)
{
    if (round_type == AES_ROUND_FIRST)
    {
        add_round_key(status, k);
        return;
    }

    sub_bytes(status, false);

    shift_rows(status);

    if (round_type != AES_ROUND_LAST)
    {
        mix_cols(status);
    }

    add_round_key(status, k);
}

void encrypt(const uint8_t k[16], const size_t k_len, const uint8_t message[16], uint8_t ciphertext[16])
{
    const size_t N_ROWS = 11;
    const size_t N_COLS = 16;

    uint8_t kr[N_ROWS * N_COLS];
    generate_round_keys(k, k_len, kr, N_ROWS, N_COLS);

    // State matrix
    uint8_t status[4 * 4];

    // State matrix: Initial status (message displaced)
    for (int i=0; i<16; i++)
    {
        status[(i % 4) * 4 + (i / 4)] = message[i];
    }

    round_encrypt(status, &kr[0 * N_COLS], AES_ROUND_FIRST);

    for (int i=1; i<10; i++)
    {
        round_encrypt(status, &kr[i * N_COLS], AES_ROUND_INTERMEDIATE);
    }

    round_encrypt(status, &kr[10 * N_COLS], AES_ROUND_LAST);

    for (int i=0; i<16; i++)
    {
        ciphertext[i] = status[(i % 4) * 4 + (i / 4)];
    }
}