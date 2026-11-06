#include <cstdint>
#include <stdio.h>

#include "lib/sbox.h"

void print_matrix(uint8_t m[4][4]) {
    printf("\n|---------------------------|\n");
    for (int i=0; i<4; i++)
    {
        printf("| ");
        for (int j=0; j<4; j++)
        {
            printf("0x%02X | ", m[i][j]);
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

void add_round_key(uint8_t status[4][4], const uint8_t k[16])
{
    // Key matrix
    uint8_t key[4][4];

    // Key matrix
    key[0][0] = k[0];
    key[1][0] = k[1];
    key[2][0] = k[2];
    key[3][0] = k[3];

    key[0][1] = k[4];
    key[1][1] = k[5];
    key[2][1] = k[6];
    key[3][1] = k[7];

    key[0][2] = k[8];
    key[1][2] = k[9];
    key[2][2] = k[10];
    key[3][2] = k[11];

    key[0][3] = k[12];
    key[1][3] = k[13];
    key[2][3] = k[14];
    key[3][3] = k[15];

    // Add Roundkey
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            uint8_t pre =  status[i][j];
            status[i][j] = status[i][j] ^ key[i][j];
        }
    }
}

void round_encrypt(uint8_t status[4][4], const uint8_t k[16], const ROUND_TYPE round_type)
{
    if (round_type == ROUND_TYPE::FIRST)
    {
        add_round_key(status, k);
        print_matrix(status);

        return;
    }

    uint8_t sbox[256];

    initialize_aes_sbox(sbox);

    // Substitution Bytes
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            status[i][j] = sbox[status[i][j]];
        }
    }

    print_matrix(status);

    // Shift Row

    // Row 0: nothing to do
    // Row 1: offset 1
    {
        const uint8_t ROW = 1;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }
    // Row 2: offset 2
    {
        const uint8_t ROW = 2;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }
    {
        const uint8_t ROW = 2;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }
    // Row 3: offset 3
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }
    {
        const uint8_t ROW = 3;
        uint8_t temp = status[ROW][0];
        status[ROW][0] = status[ROW][1];
        status[ROW][1] = status[ROW][2];
        status[ROW][2] = status[ROW][3];
        status[ROW][3] = temp;
    }

    print_matrix(status);

    if (round_type != ROUND_TYPE::LAST)
    {
        // Mix Column

        const uint8_t mix_col_mul_matrix[4][4] = {
            {0x02, 0x03, 0x01, 0x01},
            {0x01, 0x02, 0x03, 0x01},
            {0x01, 0x01, 0x02, 0x03},
            {0x03, 0x01, 0x01, 0x02}
        };

        uint8_t new_status[4][4];
        for (unsigned int i = 0; i < 4; i++)
        {
            for (unsigned int j = 0; j < 4; j++)
            {
                uint8_t acc = 0;

                for (unsigned int k = 0; k < 4; k++)
                {
                    uint8_t res = gf_mul(mix_col_mul_matrix[i][k], status[k][j]);
                    acc ^= res;
                }

                new_status[i][j] = acc;
            }
        }

        // updates original state matrix with new mixed values
        for (unsigned int i = 0; i < 4; i++)
        {
            for (unsigned int j = 0; j < 4; j++)
            {
                status[i][j] = new_status[i][j];
            }
        }

        print_matrix(status);
    }

    add_round_key(status, k);

    print_matrix(status);
}

int main(int argc, char** argv)
{
    // KEY: Thats my Kung Fu (16 ASCII characters, 1 byte each)
    uint8_t k[] = {0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75};
    
    // MESSAGE: Two One Nine Two (16 ASCII characters, 1 byte each)
    uint8_t m[] = {0x54, 0x77, 0x6F, 0x20, 0x4F, 0x6E, 0x65, 0x20, 0x4E, 0x69, 0x6E, 0x65, 0x20, 0x54, 0x77, 0x6F};

    // for (int i = 0; i < 16; i++)
    //     printf("%c", m[i]);

    const uint8_t kr[11][16] = 
    {
        {0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75},
        {0xE2, 0x32, 0xFC, 0xF1, 0x91, 0x12, 0x91, 0x88, 0xB1, 0x59, 0xE4, 0xE6, 0xD6, 0x79, 0xA2, 0x93},
        {0x56, 0x08, 0x20, 0x07, 0xC7, 0x1A, 0xB1, 0x8F, 0x76, 0x43, 0x55, 0x69, 0xA0, 0x3A, 0xF7, 0xFA},
        {0xD2, 0x60, 0x0D, 0xE7, 0x15, 0x7A, 0xBC, 0x68, 0x63, 0x39, 0xE9, 0x01, 0xC3, 0x03, 0x1E, 0xFB},
        {0xA1, 0x12, 0x02, 0xC9, 0xB4, 0x68, 0xBE, 0xA1, 0xD7, 0x51, 0x57, 0xA0, 0x14, 0x52, 0x49, 0x5B},
        {0xB1, 0x29, 0x3B, 0x33, 0x05, 0x41, 0x85, 0x92, 0xD2, 0x10, 0xD2, 0x32, 0xC6, 0x42, 0x9B, 0x69},
        {0xBD, 0x3D, 0xC2, 0x87, 0xB8, 0x7C, 0x47, 0x15, 0x6A, 0x6C, 0x95, 0x27, 0xAC, 0x2E, 0x0E, 0x4E},
        {0xCC, 0x96, 0xED, 0x16, 0x74, 0xEA, 0xAA, 0x03, 0x1E, 0x86, 0x3F, 0x24, 0xB2, 0xA8, 0x31, 0x6A},
        {0x8E, 0x51, 0xEF, 0x21, 0xFA, 0xBB, 0x45, 0x22, 0xE4, 0x3D, 0x7A, 0x06, 0x56, 0x95, 0x4B, 0x6C},
        {0xBF, 0xE2, 0xBF, 0x90, 0x45, 0x59, 0xFA, 0xB2, 0xA1, 0x64, 0x80, 0xB4, 0xF7, 0xF1, 0xCB, 0xD8},
        {0x28, 0xFD, 0xDE, 0xF8, 0x6D, 0xA4, 0x24, 0x4A, 0xCC, 0xC0, 0xA4, 0xFE, 0x3B, 0x31, 0x6F, 0x26}
    };

    // State matrix
    uint8_t status[4][4];

    // State matrix: Initial status (message displaced)
    status[0][0] = m[0];
    status[1][0] = m[1];
    status[2][0] = m[2];
    status[3][0] = m[3];

    status[0][1] = m[4];
    status[1][1] = m[5];
    status[2][1] = m[6];
    status[3][1] = m[7];
    
    status[0][2] = m[8];
    status[1][2] = m[9];
    status[2][2] = m[10];
    status[3][2] = m[11];
    
    status[0][3] = m[12];
    status[1][3] = m[13];
    status[2][3] = m[14];
    status[3][3] = m[15];

    print_matrix(status);

    printf("ROUND %d\n", 0);
    round_encrypt(status, kr[0], ROUND_TYPE::FIRST);

    for (int i=1; i<10; i++)
    {
        printf("ROUND %d\n", i);
        round_encrypt(status, kr[i], ROUND_TYPE::INTERMEDIATE);
    }

    printf("ROUND %d\n", 10);
    round_encrypt(status, kr[10], ROUND_TYPE::LAST);
}