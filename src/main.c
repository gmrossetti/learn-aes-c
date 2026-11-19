#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cli.h"
#include "aes.h"

int main(int argc, char** argv)
{
    aes_args args;

    {
        args_status status = parse_args(argc, argv, &args);

        switch (status)
        {
            case ARGS_OK:
                break;

            case ARGS_HELP:
                return EXIT_SUCCESS;

            case ARGS_ERR_COUNT:
            case ARGS_ERR_KEY:
            case ARGS_ERR_MSG:
                return EXIT_FAILURE;
        }
    }

    uint8_t c[16];
    encrypt(args.key, sizeof(args.key), args.msg, c);
    
    printf("0x");
    for (int i=0; i<16; i++)
    {
        printf("%02X", c[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}