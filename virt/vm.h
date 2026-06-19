#ifndef VM_H
#define VM_H

#include "../include/shurix.h"

#define VM_MODE_KVM      0
#define VM_MODE_EMU      1

int vm_init(void);
int vm_launch(void);
void vm_set_mode(int mode);

#endif