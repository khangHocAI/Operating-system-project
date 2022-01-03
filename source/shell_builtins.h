#ifndef __SHELLBUILTINS___
#define __SHELLBUILTINS___
#include <dirent.h>
#include <sys/wait.h> 
#include <errno.h>
#include "file_sorting.h"
#include "io_redirection.h"
void getHistory() {
    char line[80];
    int cnt = 0;
    FILE *file = fopen("history.txt", "r");
    if (file == NULL) {
        perror("history.txt");
        return;
    }
    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "") && strcmp(line, "\n") && cnt < 1000) {
            cnt++;
            printf("%i %s", cnt, line);
        } else
            break;
    }
    fclose(file);
}
void getWorkingDirectory() {
    char cwd[255];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}
void listSubDirectory() {
    char cwd[255];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return;
    }
    DIR *dir = opendir(cwd);
    struct dirent *file;
    char *fileName;
    while ((file = readdir(dir)) != NULL) {
        fileName = file->d_name;
        if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0)
            printf("%s  ", fileName);
    }
    printf("\n");
    closedir(dir);
}

void process_shell_builtins_cmd(char *args[], int *running, int iFlag,
                                int *doneBuiltins) {
    *doneBuiltins = 1;
    if (strcmp(args[0], "history") == 0) {
        getHistory();
    } else if (strcmp(args[0], "pwd") == 0)
        getWorkingDirectory();
    else if (strcmp(args[0], "ls") == 0)
        listSubDirectory();
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL)
            if (chdir(args[1]) != 0) perror(args[1]);
    }

    else if (strcmp(args[0], "sort") == 0) {
        if (args[1] != NULL && !iFlag) {
            sortFile(args[1]);
        } else {
            *doneBuiltins = 0;
        }
    } else
        *doneBuiltins = 0;
}

// void execCmd(int *args,){}

void exec(char *args[], int n, int bgFlag, int iFlag, int oFlag) {
    pid_t pid;
    if (bgFlag > 0) args[--n] = NULL;  // delete "&"

    pid = fork();
    switch (pid) {
        case -1: {  // if can not fork
            perror("forkError");
            printf("\nCan't fork!!!");
            return;
        }
        case 0: {
            if (iFlag || oFlag) {
                // printf("I/O process: ");
                if (iFlag > 0) {
                    args[iFlag - 1] = NULL;
                    set_i_mode(args, iFlag);
                }
                if (oFlag > 0) {
                    args[oFlag - 1] = NULL;
                    set_o_mode(args, oFlag);
                }
                if (execvp(args[0], args) < 0) {
                    perror("execvpError");
                    printf("\nCouldn't execute in command...");
                }
                exit(0);
            }
            default:
                if (bgFlag == 0) {
                    sigset_t sigmask;
                    sigemptyset(&sigmask);
                    sigaddset(&sigmask, SIGCHLD);
                    sigprocmask(SIG_BLOCK, &sigmask, NULL);
                    if (pid == 0) {   // Child never returns
                        for( ; ; );
                    }
                    if (sigtimedwait(&sigmask, NULL, &((struct timespec){2, 0})) < 0) {
                    if (errno == EAGAIN) {
                            printf("%s is not recognized\n", args[0]);
                            kill(pid, SIGINT);
                            return;
                    }
                }
                    waitpid(pid,0,0);
                }
                return;
        }
    }
}

void execPipe(char *args[], char *args2[], int bgFlag, int iFlag, int oFlag) {
    pid_t pid;
    int pipefd[2];  // pipefd[0] refers to the read end of the pipe.
                    // pipefd[1] refers to the write end of the pipe.

    if (pipe(pipefd) == -1) {
        perror("pipeError");
        printf("\nCan't create pipe...");
        return;
    }
    pid = fork();
    switch (pid) {
        case -1: {  // if can't fork
            perror("forkError");
            printf("\nCan't folk...");
            return;
        }
        case 0: {                            // this is child-process
            dup2(pipefd[1], STDOUT_FILENO);  // write into pipefd[1]
            close(pipefd[0]);                // close unused pipefd[0]
            close(pipefd[1]);                // close used pipefd[0]
            if (execvp(args[0], args) < 0) {
                perror("execError");
                printf("\nCan't folk...");
            }
            exit(1);
        }

        default: {  // this is parent-process
            pid = fork();
            switch (pid) {
                case -1: {
                    perror("forkError");
                    printf("\nCan't execute cmd1...");
                    return;
                }
                case 0: {
                    dup2(pipefd[0], STDIN_FILENO);  // read into pipefd[0]
                    close(pipefd[1]);               // close unused pipefd[1]
                    close(pipefd[0]);               // close used pipefd[0]

                    if (execvp(args2[0], args2) < 0) {
                        perror("execError");
                        printf("\nCannot excute cmd2...");
                    }
                    exit(1);
                }
                default: {
                    int status;
                    close(pipefd[0]);
                    close(pipefd[1]);
                    sigset_t sigmask;
                    sigemptyset(&sigmask);
                    sigaddset(&sigmask, SIGCHLD);
                    sigprocmask(SIG_BLOCK, &sigmask, NULL);
                    if (pid == 0) {   // Child never returns
                        for( ; ; );
                    }
                    if (sigtimedwait(&sigmask, NULL, &((struct timespec){2, 0})) < 0) {
                    if (errno == EAGAIN) {
                            printf("%s is not recognized\n", args[0]);
                            kill(pid, SIGINT);
                            return;
                    }
                }
                    if (bgFlag == 0) waitpid(pid, &status, 0);
                    return;
                }
            }
            break;
        }
    }
}

#endif
