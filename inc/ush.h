#pragma once
#define _POSIX_C_SOURCE 200112L //for setenv() and unsetenv()
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include "../libmx/inc/libmx.h"

extern char **environ;
void mx_type_prompt(void);
void mx_read_command(char *cmd, char **par);
int mx_input(char *str, int win_len);
void mx_execute_command(char *cmd, char *command, char **parameters, char *envp[]);

//UNSET block
int mx_unset(const char *name);
int mx_unset_check_param(char **data); //how to unset a func?
//how to check the READ_ONLY for a var name?

//ENV block
typedef struct s_flags_env
{
    bool using_I;
    bool using_U;
    bool using_P;
}              t_flags_env;
void mx_env(t_flags_env *flags, char **data);
void mx_env_flags_init(t_flags_env *data);
void mx_env_flags_set(t_flags_env *data, char **param);

//PWD block
typedef struct s_flags_pwd
{
    bool using_L;
    bool using_P;//default
}              t_flags_pwd;
void mx_pwd(t_flags_pwd *flags);
void mx_init_flags_pwd(t_flags_pwd *data);//set to default values
void mx_set_flags_pwd(t_flags_pwd *data, char **flags);//set to passing values

//CD block
void mx_cd(char *path);

