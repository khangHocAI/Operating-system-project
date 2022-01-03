#ifndef __FILESORTING__
#define __FILESORTING__
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>


void sort(char **array, int filelinecount)
{
    int i, j;
    char t[1000];

    for(i=1;i<filelinecount;i++)
    {
        for(j=1;j<filelinecount;j++)
        {
            if(strcmp(array[j-1], array[j]) > 0)
            {
                strcpy(t, array[j-1]);
                t[1000] = 0;
                strcpy(array[j-1], array[j]);
                strcpy(array[j], t);
            }
        }
    }
}
void sortFile(char* a1){
    char *lines[1000];  //limit 1000 lines
    lines[1] = "\0";
    char line[1024]; // 1 line 1024 characters
    int cnt = 0;
    FILE *file = fopen(a1, "r");
    if (file == NULL)
    {
        perror(a1);
        return;
    }
    while (fgets(line, sizeof (line), file) != NULL)
        {
            line[strcspn(line, "\n")] = '\0';
            if (cnt < sizeof (lines)/ sizeof (*lines))
                lines[cnt++] = strdup(line);
            else break;
        }
    fclose(file);
    if (strcmp(lines[1], "\0")){
        sort(lines, cnt);    
        for (int j = 0; j < cnt; j++)
            printf("%s\n", lines[j]);
        for (int j = 0; j < cnt; j++)
            free(lines[j]);
    }
}





#endif