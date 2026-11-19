#pragma once

#include <stdint.h>

typedef struct {
    uint8_t key[16];
    uint8_t msg[16];
} aes_args;

typedef enum {
    ARGS_OK = 0,        // argomenti validi, procedi
    ARGS_HELP,          // help richiesto, esci con successo
    ARGS_ERR_COUNT,     // numero di argomenti sbagliato
    ARGS_ERR_KEY,       // chiave non valida
    ARGS_ERR_MSG,       // messaggio non valido
} args_status;

void print_usage(const char *prog, const args_status arg_status);

args_status parse_args(int argc, char **argv, aes_args *out);