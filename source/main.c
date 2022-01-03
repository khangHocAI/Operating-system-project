#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "io_redirection.h"
#include "preprocessing.h"
#include "shell_builtins.h"
#define MAX_LINE 80 /* The maximum length command */

int main(void) {
    signal( SIGINT, SIG_IGN);
    signal( SIGTSTP, SIG_IGN);
    char *args[MAX_LINE / 2 + 1];
    char *args2[MAX_LINE / 2 + 1];
    char cmd[MAX_LINE];
    char cmd_to_process[MAX_LINE];
    int running = 1;
    char previous_cmd[80]="\0";
    int iFlag = 0, oFlag = 0;
    int doneBuiltins = 0;
    int pFlag = -1, bgFlag = 0, argsCount = 0;
    get_previous_command(previous_cmd);

    while (running) {
        init_shell(cmd);
        set_shell_state(args, args2, &iFlag, &oFlag, &pFlag, &bgFlag,
                        &doneBuiltins);
        save_history(cmd, previous_cmd);
        if (strcmp(cmd,"!!\n")==0){
            if (previous_cmd == NULL || previous_cmd == "\0"){
                printf("No previous command in history\n");
                continue;
            }
            else {
                strcpy(cmd, previous_cmd);
                printf("%s", cmd);
            }
        }    
        else
            strcpy(previous_cmd, cmd);
        strcpy(cmd_to_process, cmd);
        process_command(cmd_to_process, args, &argsCount, &iFlag, &oFlag,
                        &pFlag, &bgFlag);
        if (strcmp(args[0],"exit")==0)
            return 0;
        int c = args[0][0];
        if (c<50){
            continue;
        }
        process_shell_builtins_cmd(args, &running, iFlag, &doneBuiltins);
        
        if (!doneBuiltins) {
            if (pFlag == -1) {
                exec(args, argsCount, bgFlag, iFlag, oFlag);
            } else {
                int i = 0;
                for (; i < argsCount - (pFlag + 1); i++) {
                    args2[i] = args[pFlag + i + 1];
                }
                args2[i] = NULL;
                args[pFlag] = NULL;
                if(bgFlag!=0){
                    args2[i-1] = NULL;
                }
                execPipe(args, args2, bgFlag, iFlag, oFlag);
            }
        }
        strcpy(cmd,"\0");
    }
    return 0;
}
