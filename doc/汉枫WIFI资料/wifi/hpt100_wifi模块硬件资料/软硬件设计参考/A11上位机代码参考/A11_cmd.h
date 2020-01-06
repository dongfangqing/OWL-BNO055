#ifndef A11_CMD_H
#define A11_CMD_H

int A11_connected;

void connect_A11(HANDLE fd);
int module_send_cmd_set(HANDLE fd, char *cmd);
int module_send_cmd_get(HANDLE fd, char *cmd, char *ret);

#endif

