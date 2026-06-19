#ifndef DENITRON_H
#define DENITRON_H

#include "../include/shurix.h"
#include "../lib/string.h"

size_t denitron_ray(unsigned int ram_size_bytes, const unsigned char* signature, size_t sig_len);
void denitron_scan_process_list(unsigned int ram_size_bytes, const char* proc_name);

#endif
