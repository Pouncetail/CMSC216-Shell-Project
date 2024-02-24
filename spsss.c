/*
Cedric Yeo, cyeo@grace.umd.edu, 118859781, 0201
I pledge on my honor that I have not given or received any unauthorized
assistance on this examination
*/
/* These are all of the library used in the project.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>
#include <sysexits.h>
#include "spsss.h"
#include "safe-fork.h"
#include "split.h"

static void clear_commands(Command *const head_cmd);

Spsss_commands read_spsss_commands(const char compile_cmds[],
                                   const char test_cmds[]){
    Spsss_commands ret;
    Command *curr = NULL;
    FILE *read_comp_cmds = NULL, *read_test_cmds = NULL;
    char *temp = NULL;
    if (compile_cmds == NULL || test_cmds == NULL
        || (read_comp_cmds = fopen(compile_cmds, "r")) == NULL
        || (read_test_cmds = fopen(test_cmds, "r")) == NULL){
        exit(1);
    }
    if ((temp = malloc(LINE_MAX + 1)) == NULL){
        perror("Memory not allocated.\n");
        exit(1);
    }
    ret.head_comp_cmds = NULL;
    while (fgets(temp, LINE_MAX + 1, read_comp_cmds) != NULL){
        if (ret.head_comp_cmds == NULL){
            if ((ret.head_comp_cmds = malloc(sizeof(Command)))== NULL){
                perror("Memory not allocated.\n");
                exit(1);
            }
            curr = ret.head_comp_cmds;
        } else {
            if ((curr->next_cmd = malloc(sizeof(Command))) == NULL){
                perror("Memory not allocated.\n");
                exit(1);
            }
            curr = curr->next_cmd;
        }
        if ((curr->cmd_line = malloc(LINE_MAX + 1)) == NULL){
            perror("Memory not allocated.\n");
            exit(1);
        }
        strcpy(curr->cmd_line, temp);
        curr->next_cmd = NULL;
    }
    
    ret.head_test_cmds = NULL;
    while (fgets(temp, LINE_MAX + 1, read_test_cmds) != NULL){
        if (ret.head_test_cmds == NULL){
            if ((ret.head_test_cmds = malloc(sizeof(Command)))== NULL){
                perror("Memory not allocated.\n");
                exit(1);
            }
            curr = ret.head_test_cmds;
        } else {
            if ((curr->next_cmd = malloc(sizeof(Command))) == NULL){
                perror("Memory not allocated.\n");
                exit(1);
            }
            curr = curr->next_cmd;
        }
        if ((curr->cmd_line = malloc(LINE_MAX + 1)) == NULL){
            perror("Memory not allocated.\n");
            exit(1);
        }
        strcpy(curr->cmd_line, temp);
        curr->next_cmd = NULL;
    }
    fclose(read_comp_cmds);
    fclose(read_test_cmds);
    free(temp);
    return ret;
}

void clear_spsss_commands(Spsss_commands *const commands){
    if (commands == NULL){
        return;
    }
    if (commands->head_comp_cmds != NULL){
        clear_commands(commands->head_comp_cmds);
    }
    if (commands->head_test_cmds != NULL){
        clear_commands(commands->head_test_cmds);
    }
}

static void clear_commands(Command *const head_cmd){
    Command *curr = NULL, *next = NULL;
    curr = head_cmd->next_cmd;
    free(head_cmd->cmd_line);
    free(head_cmd);
    while (curr != NULL){
        next = curr->next_cmd;
        free(curr->cmd_line);
        free(curr);
        curr = next;
    }
}

int compile_program(Spsss_commands commands){
    Command *curr = NULL;
    char **argv = NULL;
    int status = 0, i, op_num;
    pid_t pid;
    if (commands.head_comp_cmds != NULL){
        curr = commands.head_comp_cmds;
        while (curr != NULL){
            i = 0;
            argv = split(curr->cmd_line);
            pid = safe_fork();
            if (pid == -1){
                perror("Fork not made.\n");
                while (argv[i] != NULL){
                    free(argv[i]);
                    i++;
                }
                free(argv);
                exit(1);
            } else if (pid == 0){
                while (argv[i] != NULL){
                    if (*argv[i] == '>'){
                        if ((op_num = open(argv[i + 1],
                             FILE_FLAGS, FILE_MODE)) < 0){
                            perror("could not open file.\n");
                            exit(1);
                        }
                        dup2(op_num, STDOUT_FILENO);
                        free(argv[i]);
                        free(argv[i + 1]);
                        i += 2;
                    } else if (*argv[i] == '<'){
                        if ((op_num = open(argv[i + 1], O_RDONLY)) < 0){
                            perror("could not open file.\n");
                            exit(1);
                        }
                        dup2(op_num, STDIN_FILENO);
                        free(argv[i]);
                        free(argv[i + 1]);
                        i += 2;
                    } else {
                        i++;
                    }
                }
                execvp(argv[0], argv);
            } else {
                wait(&status);
                while (argv[i] != NULL){
                    free(argv[i]);
                    i++;
                }
                free(argv);
                if (WIFEXITED(status)){
                    if (WEXITSTATUS(status) != 0){
                        return FAILED_COMPILATION;
                    }
                } else {
                    return FAILED_COMPILATION;
                }
            }
            curr = curr->next_cmd;
        }
    }
    return SUCCESSFUL_COMPILATION;
}
int test_program(Spsss_commands commands){
    Command *curr = NULL;
    char **argv = NULL;
    int ret = 0, status = 0, i = 0, op_num;
    pid_t pid;
    if (commands.head_test_cmds != NULL){
        curr = commands.head_test_cmds;
        while (curr != NULL){
            i = 0;
            argv = split(curr->cmd_line);
            pid = safe_fork();
            if (pid == -1){
                perror("Fork not made.\n");
                while (argv[i] != NULL){
                    free(argv[i]);
                    i++;
                }
                free(argv);
                exit(1);
            } else if (pid == 0){
                while (argv[i] != NULL){
                    if (*argv[i] == '>'){
                        if ((op_num = open(argv[i + 1],
                             FILE_FLAGS, FILE_MODE)) < 0){
                            perror("could not open file.\n");
                            exit(1);
                        }
                        dup2(op_num, STDOUT_FILENO);
                        free(argv[i]);
                        free(argv[i + 1]);
                        i += 2;
                    } else if (*argv[i] == '<'){
                        if ((op_num = open(argv[i + 1], O_RDONLY)) < 0){
                            perror("could not open file.\n");
                            exit(1);
                        }
                        dup2(op_num, STDIN_FILENO);
                        free(argv[i]);
                        free(argv[i + 1]);
                        i += 2;
                    } else {
                        i++;
                    }
                }
                execvp(argv[0], argv);
            } else {
                wait(&status);
                while (argv[i] != NULL){
                    free(argv[i]);
                    i++;
                }
                free(argv);
                if (WIFEXITED(status)){
                    if (WEXITSTATUS(status) == 0){
                        ret++;
                    }
                }
            }
            curr = curr->next_cmd;
        }
    }
    return ret;
}