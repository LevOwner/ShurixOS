#ifndef DENITRON_H
#define DENITRON_H

#include "../include/shurix.h"

int denitron_validate(const char *name, const char *dest, const char *intent);
void denitron_register(uint32_t pid, const char *name, const char *dest, const char *intent);
void denitron_destroy(uint32_t pid);
void denitron_scan_all(void);
void denitron_status(void);

#endif