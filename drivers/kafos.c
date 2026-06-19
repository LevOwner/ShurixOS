#include "kafos.h"
#include "../include/shurix.h"
#include "../container/container.h"
#include "../ui/menu.h"
#include "../virt/vm.h"
#include "../lib/string.h"

int kafos_send(kafos_command_t *cmd, kafos_response_t *resp) {
    if (!cmd) {
        resp->result = -1;
        return -1;
    }
    return kafos_handle(cmd, resp);
}

int kafos_handle(kafos_command_t *cmd, kafos_response_t *resp) {
    resp->result = 0;

    switch (cmd->cmd) {
        case KAFOS_CMD_START_VM: {
            print_color("KAFOS: starting VM...\n", 0x0A);
            resp->result = vm_launch();
            if (resp->result == 0) {
                strcpy(resp->message, "VM started");
            } else {
                strcpy(resp->message, "VM start failed");
            }
            break;
        }

        case KAFOS_CMD_STOP_VM: {
            print_color("KAFOS: stopping VM...\n", 0x0C);
            strcpy(resp->message, "VM stopped");
            break;
        }

        case KAFOS_CMD_CREATE_CONTAINER: {
            print_color("KAFOS: creating container...\n", 0x0A);
            int ret = container_create(cmd->name, cmd->param1, cmd->param2);
            if (ret == 0) {
                strcpy(resp->message, "Container created");
            } else {
                strcpy(resp->message, "Container creation failed");
                resp->result = -1;
            }
            break;
        }

        case KAFOS_CMD_LIST_CONTAINERS: {
            container_list();
            strcpy(resp->message, "Container list displayed");
            break;
        }

        case KAFOS_CMD_GET_STATUS: {
            strcpy(resp->message, "System: OK");
            break;
        }

        default: {
            strcpy(resp->message, "Unknown KAFOS command");
            resp->result = -1;
            break;
        }
    }

    return resp->result;
}