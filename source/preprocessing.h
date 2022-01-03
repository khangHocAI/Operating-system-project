#ifndef __PREPROCESSING___
#define __PREPROCESSING___
#include <stdio.h>
#include <string.h>
#define MAX_LINE 80 /* The maximum length command */

void stopCtrlC( int signo )
{
    if( signo == SIGINT )
    {
        fflush(stdout);
    }
}
void stopCtrlZ( int signo)
{
    if( signo == SIGTSTP )
    {
        fflush(stdout);
    }
}

void get_previous_command(char *cmd) {
    strcpy(cmd, "\n");
    char tmp[80];
    FILE *fp;
    fp = fopen("history.txt", "a+");
    while (fgets(tmp, sizeof(tmp), fp)) {
        if (!strcmp(tmp, "") || !strcmp(tmp, "\n")) break;
        strcpy(cmd, tmp);
    }
    fclose(fp);
}
void init_shell(char *cmd) {
    printf("osh>");
    fflush(stdout);
    fgets(cmd, MAX_LINE, stdin);
}
void set_shell_state(char *args[], char *args2[], int *iFlag, int *oFlag,
                     int *pFlag, int *bgFlag, int *doneBuiltins) {
    for (int i = 0; i < (MAX_LINE / 2 + 1); i++) {
        args[i] = '\0';
        args2[i] = '\0';
    }
    *iFlag = 0;
    *oFlag = 0;
    *pFlag = -1;
    *bgFlag = 0;
    *doneBuiltins = 0;
}
void save_history(char *cmd, char *history) {
    if (strcmp(cmd, history) != 0) {
        FILE *fp = fopen("history.txt", "a+");
        fputs(cmd, fp);  // save to history file
        fclose(fp);
    }
}

void process_command(char *cmd, char *args[], int *argsCount, int *iFlag,
                     int *oFlag, int *pFlag, int *bgFlag) {
    strtok(cmd, "\n");
    char *token = strtok(cmd, " ");
    int i = 0;
    args[i] = token;
    // printf("Tokens[%d]: %s \n",i, token);
    while (token != NULL) {
        if (!strcmp(token, "<"))
            *iFlag = i + 1;
        else if (!strcmp(token, ">"))
            *oFlag = i + 1;
        else if (strcmp(token, "|") == 0)
            *pFlag = i;
        else if (strcmp(token, "&") == 0) {
            *bgFlag = i;
            // printf("bgFlagChecked");
        }
        args[i++] = token;
        token = strtok(NULL, " ");
        // printf("Tokens[%d]: %s \n",i, token);
    }
    *argsCount = i;
}
#endif
