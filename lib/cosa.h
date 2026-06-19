#ifndef COSA_H
#define COSA_H

#include "../include/shurix.h"

int cosa_pack(const void *data, uint64_t size, const char *filename);
int cosa_unpack(const char *filename, void *buffer, uint64_t max_size);

#endif