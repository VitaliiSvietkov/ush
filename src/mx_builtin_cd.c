#include "../inc/ush.h"

static void go_to_parent();
static void go_to_dir(char *dir);
static int check_lnk_path(char *path);

void mx_builtin_cd(char **params, t_flags_cd *flags) {
    int argc = 0;
    for (; params[argc] != NULL; argc++);

    switch (argc)
    {
    case 1:
        if (t_global.HOME[0] != '\0') {
            chdir(t_global.HOME);
            if (mx_strcmp(t_global.OLDPWD, t_global.PWD)) {
                mx_memcpy(t_global.OLDPWD, t_global.PWD, PATH_MAX);
                setenv("OLDPWD", t_global.OLDPWD, 1);
            }
            mx_memcpy(t_global.PWD, t_global.HOME, PATH_MAX);
            setenv("PWD", t_global.PWD, 1);
        }
        return;
        break;
    case 2:
        if (params[1][0] == '-' && mx_strlen(params[1]) > 1) {
            if (t_global.HOME[0] != '\0') {
                chdir(t_global.HOME);
                if (mx_strcmp(t_global.OLDPWD, t_global.PWD)) {
                    mx_memcpy(t_global.OLDPWD, t_global.PWD, PATH_MAX);
                    setenv("OLDPWD", t_global.OLDPWD, 1);
                }
                mx_memcpy(t_global.PWD, t_global.HOME, PATH_MAX);
                setenv("PWD", t_global.PWD, 1);
            }
            return;
        }
        else  if (params[1][0] == '-') {
            if (t_global.OLDPWD[0] != '\0') {
                chdir(t_global.OLDPWD);
                mx_memcpy(t_global.PWD, t_global.OLDPWD, PATH_MAX);
                setenv("PWD", t_global.PWD, 1);
            }
            return;
        }
        break;
    case 3:

        break;
    case 4:

        break;
    default:
        break;
    }    

    char real_buf[PATH_MAX];
    char *path = NULL;

    char tmp_PWD[PATH_MAX];
    mx_memcpy(tmp_PWD, t_global.PWD, PATH_MAX);

    if (params[1][0] == '-' && mx_strlen(params[1]) > 1)
        path = mx_strdup(params[2]);
    else
        path = mx_strdup(params[1]);

    if (flags->using_s) {
        if (check_lnk_path(path)) {
            mx_printerr("ush: cd: not a directory: ");
            mx_printerr(path);
            mx_printerr("\n");
            if (path != NULL)
                free(path);
            return;
        }
        if (path != NULL)
            free(path);
        return;
    }

    if (path != NULL && flags->using_P) {
        char *res = NULL;
        res = realpath(path, real_buf);
        path = mx_strdup(real_buf);
        if (!res)
            free(res);
    }

    int status;
    if (path[0] == '/') {
        mx_memset(t_global.PWD, '\0', mx_strlen(t_global.PWD));
        t_global.PWD[0] = '/';
    }

    char **arr = mx_strsplit(path, '/');
    for (int i = 0; arr[i] != NULL; i++) {
        if (!mx_strcmp(arr[i], ".."))
            go_to_parent();
        else if (mx_strcmp(arr[i], "."))
            go_to_dir(arr[i]);
    }
    mx_del_strarr(&arr);

    status = chdir(t_global.PWD);
    if (status < 0) {
        mx_printerr("ush: cd: ");
        perror(path);
        mx_memcpy(t_global.PWD, tmp_PWD, PATH_MAX);
    }
    else {
        if (mx_strcmp(t_global.OLDPWD, tmp_PWD)) {
            mx_memcpy(t_global.OLDPWD, tmp_PWD, PATH_MAX);
            status = setenv("OLDPWD", t_global.OLDPWD, 1);
        }
        status = setenv("PWD", t_global.PWD, 1);
    }

    if (path != NULL)
        free(path);
}

/*
    Remove the name of the last directory in the t_global.PWD variable
*/
static void go_to_parent() {
    char *ptr = strrchr(t_global.PWD, '/');
    if (ptr)
        mx_memset(ptr, '\0', mx_strlen(ptr));
    else
        t_global.PWD[0] = '/';
    
    if (t_global.PWD[0] == '\0')
        t_global.PWD[0] = '/';
}

/*
    Append to the t_global.PWD variable the name of the directory, specified in the 'dir'
*/
static void go_to_dir(char *dir) {
    int len = mx_strlen(t_global.PWD);
    if (t_global.PWD[len - 1] != '/')
        t_global.PWD[mx_strlen(t_global.PWD)] = '/';
    mx_memcpy(t_global.PWD + mx_strlen(t_global.PWD), dir, mx_strlen(dir));
}


/*
    Execute the same algorithm as in the main body, 
    but checking every directory on the path for link
   
    Return value: 1 if link was found, 0 otherwise
*/
static int check_lnk_path(char *path) {
    char tmp_lnk_PWD[PATH_MAX];
    mx_memcpy(tmp_lnk_PWD, t_global.PWD, PATH_MAX);
    if (path[0] == '/') {
        memset(t_global.PWD, 0, PATH_MAX);
        t_global.PWD[0] = '/';
    }

    char buf[PATH_MAX];
    memset(buf, 0, PATH_MAX);
    ssize_t nbytes = 0;
    char **lnk_arr = mx_strsplit(path, '/');

    for (int i = 0; lnk_arr[i] != NULL; i++) {
        if (!mx_strcmp(lnk_arr[i], ".."))
            go_to_parent();
        else if (mx_strcmp(lnk_arr[i], "."))
            go_to_dir(lnk_arr[i]);

        nbytes = readlink(t_global.PWD, buf, PATH_MAX);
        if (nbytes > 0 && mx_strcmp(lnk_arr[i], "..") && mx_strcmp(lnk_arr[i], ".")) {
            mx_del_strarr(&lnk_arr);
            mx_memcpy(t_global.PWD, tmp_lnk_PWD, PATH_MAX);
            return 1;
        }
        mx_memset(buf, 0, mx_strlen(buf));
    }
    mx_del_strarr(&lnk_arr);

    int status = chdir(t_global.PWD);
    if (status < 0) {
        mx_printerr("ush: cd: ");
        perror(path);
        mx_memcpy(t_global.PWD, tmp_lnk_PWD, PATH_MAX);
    }
    else {
        if (mx_strcmp(t_global.OLDPWD, tmp_lnk_PWD)) {
            mx_memcpy(t_global.OLDPWD, tmp_lnk_PWD, PATH_MAX);
            status = setenv("OLDPWD", t_global.OLDPWD, 1);
        }
        status = setenv("PWD", t_global.PWD, 1);
    }

    return 0;
}
