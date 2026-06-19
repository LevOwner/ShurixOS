#ifndef KAFOS_H
#define KAFOS_H

#include "../include/shurix.h"

#define KAFOS_CMD_START_VM         1
#define KAFOS_CMD_STOP_VM          2
#define KAFOS_CMD_CREATE_CONTAINER 3
#define KAFOS_CMD_LIST_CONTAINERS  4
#define KAFOS_CMD_GET_STATUS       5

typedef struct {
    uint32_t cmd;
    char name[32];
    uint32_t param1;
    uint32_t param2;
} kafos_command_t;

typedef struct {
    int result;
    char message[64];
} kafos_response_t;

int kafos_send(kafos_command_t *cmd, kafos_response_t *resp);
int kafos_handle(kafos_command_t *cmd, kafos_response_t *resp);

#endif