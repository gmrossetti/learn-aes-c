#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    uint8_t key[16];
    uint8_t msg[16];
} aes_args;

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

int parse_args(int argc, char **argv, aes_args *out)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <key:16 bytes> <message:16 bytes>\n", argv[0]);
        return -1;
    }

    if (parse_block("key", argv[1], out->key) != 0) return -1;
    if (parse_block("message", argv[2], out->msg) != 0) return -1;
    return 0;
}