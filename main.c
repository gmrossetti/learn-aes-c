#include <stdio.h>
#include <cstdint>

#include "lib/aes.h"

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



int main(int argc, char** argv)
{
    // KEY: Thats my Kung Fu (16 ASCII characters, 1 byte each)
    const uint8_t k[] = {0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75};
    
    // MESSAGE: Two One Nine Two (16 ASCII characters, 1 byte each)
    const uint8_t m[] = {0x54, 0x77, 0x6F, 0x20, 0x4F, 0x6E, 0x65, 0x20, 0x4E, 0x69, 0x6E, 0x65, 0x20, 0x54, 0x77, 0x6F};

    uint8_t c[16];
    encrypt(k, sizeof(k), m, c);
    
    printf("0x");
    for (int i=0; i<16; i++)
    {
        printf("%02X", c[i]);
    }
    printf("\n");
}