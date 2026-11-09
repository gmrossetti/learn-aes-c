#include <cstdint>
#include <stdio.h>

#include "lib/sbox.h"
#include "lib/key_schedule.h"

void print_matrix(uint8_t m[4 * 4]) {
    printf("\n|---------------------------|\n");
    for (int i=0; i<4; i++)
    {
        printf("| ");
        for (int j=0; j<4; j++)
        {
            printf("0x%02X | ", m[i * 4 + j]);
        }
        printf("\n");
    }
    printf("|---------------------------|\n\n");
}

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

enum ROUND_TYPE {
    FIRST,
    INTERMEDIATE,
    LAST,
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

    static uint8_t sbox[256];
    static int sbox_initialized = false;

    if (!sbox_initialized)
    {
        initialize_aes_sbox(sbox);
        sbox_initialized = true;
    }

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

void shift_rows(uint8_t status[16])
{
    // Shift Row

    // Row 0: nothing to do
    // Row 1: offset 1
    {
        const uint8_t ROW = 1;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
    }
    // Row 2: offset 2
    {
        const uint8_t ROW = 2;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
    }
    {
        const uint8_t ROW = 2;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
    }
    // Row 3: offset 3
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
    }
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
    }
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW * 4 + 0];
        status[ROW * 4 + 0] = status[ROW * 4 + 1];
        status[ROW * 4 + 1] = status[ROW * 4 + 2];
        status[ROW * 4 + 2] = status[ROW * 4 + 3];
        status[ROW * 4 + 3] = temp;
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

void round_encrypt(uint8_t status[], const uint8_t k[16], const ROUND_TYPE round_type)
{
    if (round_type == ROUND_TYPE::FIRST)
    {
        add_round_key(status, k);
        print_matrix(status);

        return;
    }

    sub_bytes(status, false);

    print_matrix(status);

    shift_rows(status);

    print_matrix(status);

    if (round_type != ROUND_TYPE::LAST)
    {
        mix_cols(status);

        print_matrix(status);
    }

    add_round_key(status, k);

    print_matrix(status);
}

int main(int argc, char** argv)
{
    // KEY: Thats my Kung Fu (16 ASCII characters, 1 byte each)
    const uint8_t k[] = {0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75};
    
    // MESSAGE: Two One Nine Two (16 ASCII characters, 1 byte each)
    const uint8_t m[] = {0x54, 0x77, 0x6F, 0x20, 0x4F, 0x6E, 0x65, 0x20, 0x4E, 0x69, 0x6E, 0x65, 0x20, 0x54, 0x77, 0x6F};

    const size_t N_ROWS = 11;
    const size_t N_COLS = 16;

    uint8_t kr[N_ROWS * N_COLS];
    generate_round_keys(k, sizeof(k), kr, N_ROWS, N_COLS);

    // State matrix
    uint8_t status[4 * 4];

    // State matrix: Initial status (message displaced)
    status[0 * 4 + 0] = m[0];
    status[1 * 4 + 0] = m[1];
    status[2 * 4 + 0] = m[2];
    status[3 * 4 + 0] = m[3];

    status[0 * 4 + 1] = m[4];
    status[1 * 4 + 1] = m[5];
    status[2 * 4 + 1] = m[6];
    status[3 * 4 + 1] = m[7];
    
    status[0 * 4 + 2] = m[8];
    status[1 * 4 + 2] = m[9];
    status[2 * 4 + 2] = m[10];
    status[3 * 4 + 2] = m[11];
    
    status[0 * 4 + 3] = m[12];
    status[1 * 4 + 3] = m[13];
    status[2 * 4 + 3] = m[14];
    status[3 * 4 + 3] = m[15];

    print_matrix(status);

    printf("ROUND %d\n", 0);
    round_encrypt(status, &kr[0 * N_COLS], ROUND_TYPE::FIRST);

    for (int i=1; i<10; i++)
    {
        printf("ROUND %d\n", i);
        round_encrypt(status, &kr[i * N_COLS], ROUND_TYPE::INTERMEDIATE);
    }

    printf("ROUND %d\n", 10);
    round_encrypt(status, &kr[10 * N_COLS], ROUND_TYPE::LAST);
}