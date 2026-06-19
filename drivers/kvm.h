#ifndef KVM_H
#define KVM_H

#include "../include/shurix.h"

int kvm_init(void);
int kvm_start_container(container_t *c);
int kvm_stop_container(container_t *c);

#endif