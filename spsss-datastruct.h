#if !defined(SPSSS_DATASTRUCT_H)
#define SPSSS_DATASTRUCT_H

/*
Cedric Yeo, cyeo@grace.umd.edu, 118859781, 0201
I pledge on my honor that I have not given or received any unauthorized
assistance on this examination
*/
/* Datastructure for the list of spsss-datastruct; used 2 linked lists*/
/* These are all of the library used in the project.*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "split.h"
#include "safe-fork.h"
/* 2 linked lists for compile commands and test commands (heads)*/
typedef struct spsss_commands {
    struct command *head_comp_cmds;
    struct command *head_test_cmds;
} Spsss_commands;

/* each command has a command line assosicated with it,
as well as the next command.*/
typedef struct command {
    char *cmd_line;
    struct command *next_cmd;
} Command;

#endif