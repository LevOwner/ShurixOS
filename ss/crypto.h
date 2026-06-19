#ifndef CRYPTO_H
#define CRYPTO_H

#include "../include/shurix.h"

void crypto_derive_key(const char *password);
void crypto_encrypt_block(uint8_t *data);
void crypto_decrypt_block(uint8_t *data);
void crypto_encrypt_disk(uint8_t *buffer, uint32_t sector, uint32_t count);
void crypto_decrypt_disk(uint8_t *buffer, uint32_t sector, uint32_t count);

#endif