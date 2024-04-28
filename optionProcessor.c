/**
 * @file optionProcessor.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief Functions for dealing with commands inputted
 * @version 0.1
 * @date 2022-02-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>

bool checkIfValidPID(char * inputPID)
{

}

// Helper function to check if a struct dirent from /proc is a PID folder.
int is_pid_folder(const struct dirent *entry) 
{
    const char *p;

    for (p = entry->d_name; *p; p++) 
    {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
}

char * parseStat(char * inputPID)
{
    char arrStatValues[3];
}

char * parseStatm(char * inputPID)
{
    char arrStatmValues[2];
}

char parseCmdline(char * inputPID)
{
    
}

int getPID(bool PIDState, char * theNum)
{
    if (PIDState == true)
    {
        printf("we made it this far \n");
    }

    //DIR *procdir;
    FILE *fp;
    //struct dirent *entry;
    char path[256 + 5 + 5]; // d_name + /proc + /stat

    // Open /proc directory.
    // procdir = opendir("/proc");
    // if (!procdir) 
    // {
    //     perror("opendir failed");
    //     return 1;
    // }

    // Iterate through all files and folders of /proc.
    // while ((entry = readdir(procdir))) 
    // {
    //     // Skip anything that is not a PID folder.
    //     if (!is_pid_folder(entry))
    //         continue;

        // Try to open /proc/<PID>/stat.
        snprintf(path, sizeof(path), "/proc/%s/stat", theNum);
        fp = fopen(path, "r");

        if (!fp) 
        {
            perror(path);
            //continue;
        }


        char str[10000];
        char arrayOfLines[49];
        const char s[2] = " ";
        char *token;
        int i = 0;
        
        while(fgets(str, sizeof(str) / sizeof(str[0]), fp) != NULL)
        {
            char *ptr1 = strtok(str, " ");
            strcpy(arrayOfLines[i], ptr1);
            i++;
        }
        printf(arrayOfLines);

        // /* get the first token */
        // token = strtok(str, s);
        
        // /* walk through other tokens */
        // while( token != NULL ) 
        // {
        //     printf( " %s\n", token );
            
        //     token = strtok(NULL, s);
        // }

    //}

    return 0;
}

char getStateInfo(bool StateInfo, char * theNum)
{

}

char getUserTime(bool UTimeInfo, char * theNum)
{

}

char getSysTime(bool SysTimeInfo, char * theNum)
{

}

char getVMem(bool VMemInfo, char * theNum)
{

}

char getCommandLine(bool cmdInfo, char * theNum)
{

}