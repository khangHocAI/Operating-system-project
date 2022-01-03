#ifndef __IOREDIRECTION___
#define __IOREDIRECTION___
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void set_o_mode(char* args[], int oFlag) {
    int out = open(args[oFlag], O_WRONLY | O_TRUNC | O_CREAT,
                   S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if (out < 0) {
        perror(args[oFlag]);
        return;
    }
    if (dup2(out, STDOUT_FILENO) < 0) {
        perror("cannot redirect stdout");
        return;
    }
    close(out);
    return;
}

void reset_o_mode(char* args[], int oFlag, int* std) {
    if (oFlag && *std) {
        dup2(*std, fileno(stdout));
        close(*std);
    }
}

void set_i_mode(char* args[], int iFlag) {
    int in = open(args[iFlag], O_RDONLY);
    if (in < 0) {
        perror(args[iFlag]);
        return;
    }
    if (dup2(in, STDIN_FILENO) < 0) {
        perror("cannot redirect stdout");
        return;
    }
    close(in);
    return;
}
#endif
