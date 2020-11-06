#pragma once
//#include <stdio.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <string.h>
#include "../libmx/inc/libmx.h"

typedef struct s_commands
{
    bool using_pwd;
}              t_commands;

void mx_type_prompt(void);
void mx_read_command(char *cmd, char **par);
int mx_input(char *str, int win_len);
void mx_commands_init(t_commands *commands);
void mx_commands_set(t_commands *commands, char **arr);
