#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static int parse_block(const char *name, const char *s, uint8_t out[16])
{
    size_t len = strlen(s);
    if (len != 16) {
        fprintf(stderr, "Invalid %s: expected 16 bytes, got %zu\n", name, len);
        return -1;
    }
    memcpy(out, s, 16);
    return 0;
}

void print_usage(const char *prog, const args_status arg_status)
{
    if (arg_status == ARGS_HELP)
    {
        fprintf(stdout, "Usage: %s <key:16 bytes> <message:16 bytes>\n", prog);
        return;
    }

    fprintf(stderr, "Usage: %s <key:16 bytes> <message:16 bytes>\n", prog);
}

args_status parse_args(int argc, char **argv, aes_args *out)
{
    if (argc == 2)
    {
        const bool is_help_req = strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0;

        if (is_help_req)
        {
            const args_status status = ARGS_HELP;
            print_usage(argv[0], status);
            return status;
        }
    }

    if (argc != 3)
    {
        const args_status status = ARGS_ERR_COUNT;
        print_usage(argv[0], status);
        return status;
    }

    if (parse_block("key", argv[1], out->key) != 0) return ARGS_ERR_KEY;
    if (parse_block("message", argv[2], out->msg) != 0) return ARGS_ERR_MSG;

    return ARGS_OK;
}