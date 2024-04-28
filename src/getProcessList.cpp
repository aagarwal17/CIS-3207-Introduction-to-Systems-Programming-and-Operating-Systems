/**
 * @file parser.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief 
 * @version 0.1
 * @date 2022-02-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h> 
#include <unistd.h> 
#include <stdbool.h>
#include "optionProcessor.h"
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

void getAllPIDparser(char *thePID);


void findAllPID()
{
    struct dirent *de;  // Pointer for directory entry
  
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir("/proc");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
    }
  
    const char *p;
    
    char * myUIDArr[100];

    for(int j = 0; j < 100; j++)
    {
        myUIDArr[j] = (char *)malloc(1000);
    }


    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        p = de->d_name;
        isUserId(de->d_name, myUIDArr);
        if (isdigit(*p))
        { 
            getAllPIDparser(de->d_name);
        }
    }
    closedir(dr);    
    //return 0;
}


void getAllPIDparser(char *thePID)
{
    char * myAllPidArray[4];
    char vMemAll[1000];
    char * myAllCmdArr[50];

    for(int i = 0; i < 4; i++)
    {
        myAllPidArray[i] = (char *)malloc(100);
    }

    for(int j = 0; j < 50; j++)
    {
        myAllCmdArr[j] = (char *)malloc(2000);
    }

    //printf("\nThe Inputted PID: %s\n", thePID);
    parseStat(thePID, myAllPidArray);
    printf("PID: %s\t Size: %s\t UTime: %s\t STime: %s", myAllPidArray[0], myAllPidArray[1], myAllPidArray[2], myAllPidArray[3]);
    getVMem(vMemAll, thePID);
    printf("\tVMem: %s", vMemAll);
    getCommandLine(myAllCmdArr, thePID);
    int c; 
    printf("\tCmdline: ");
    for(c = 0;c<50;c++)
    {
        printf("%s ", myAllCmdArr[c]);	//print array 
    } 
    printf("\n");

}