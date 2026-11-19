#pragma once

#include <stdint.h>

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

const uint8_t *aes_box();