/**
 * @file getProcessList.cpp
 * @author Arun Agarwa; (tuk82261@temple.edu)
 * @brief My file to parse the command line if no -p is provided, thus printing out all pids of the current user based on the inputted flags. It will also print out a few things for nice formatting
 * @version 0.1
 * @date 2022-02-14
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
#include <sys/stat.h>

/**
 * @brief Global variables to hold true/false values for parser/getopt function
 * 
 */
bool procState = false;
bool uTime = true;
bool sysTime = false;
bool vMemory = false;
bool dispCMD = true;


void getAllPIDparser(char *thePID); //this function exists below but needs to have his function header included at the top because the parser function calls this function

/**
 * @brief Finds all pids in the /proc folder, making sure to check that they are 1) numbers and 2) pids of the current user
 * 
 */
void findAllPID()
{
    //A simple print statement letting the user know in the terminal what we are doing
    printf("No -p command found with pid arg--printing all process id's and information for current user based on command line args:\n\n");
    struct dirent *de;  // Pointer for directory entry
  
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir("/proc");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
    }
  
    char *p = (char *)malloc(100);

    //Read through proc file by file until NULL/the end, making sure to check that the pid is a number and a pid of the current user
    while ((de = readdir(dr)) != NULL)
    {
        strcpy(p,de->d_name);

        bool tf = false;
        long unsigned int i;
        for (i=0;i<strlen(p)-1;i++)
        {
            tf=isdigit(p[i]); //checking that pid is a digit/number
        }
        if(tf)
        {
            if (isUserId(p) == 0) //calls isUserId function from optionProcessor.c, which returns 0 if the pid belongs to current 0, and returns 1 if not
                getAllPIDparser(de->d_name); //if this is the case call getAllPIDParser function below to print out the desired information to the terminal
        }
    }
    closedir(dr);    
}

/**
 * @brief My second parser function made specially for the case that no -p flag is provided; takes input from the main function to parse the command line and output the desired flag information for all current user pids
 * 
 * @param argCount number of args inputtted into the terminal
 * @param args the array of strings inputted into the terminal
 */
void allParser(int argCount, char **args)
{
    //local variable to be used for different flags
    int opt;

    // put ':' in the starting of the string so that program can distinguish between '?' and ':' 
    while((opt = getopt(argCount, args, ":sUSvc")) != -1)
    {
        switch(opt)
        {
            case 's': //if -s inputted, set boolean processState to true
                procState = true;
                break;
            case 'U': //if -U inputted, set boolean userTime to false because we do not want to print this out if it included
                uTime = false;
                break;
            case 'S': //if -S inputted, set boolean systemTime to true
                sysTime = true;
                break;
            case 'v': //if -v inputted, set boolean vMemUsed to true; also, check to make sure pid is valid and find the vMemory
                vMemory = true;
                break;
            case 'c': //if -c inputted, set boolean displaycmd to false because we do not want to print out the cmdline if the flag is included
                dispCMD = false;
                break;
            case '?': //if some invalid flag is inputted, print out the following error type message
                fprintf(stderr, "Unknown Option: '-%c'.\n", optopt);
                break;    
        }        
    }
    
    //This optind is for the extra arguments which are not parsed
    for(; optind < argCount; optind++)
    {     
        printf("extra arguments: %s\n", args[optind]); 
    }
    findAllPID(); //call findAllPID function above to find all the PIDS of current user
}


/**
 * @brief While the name is a bit misleading, this function prints out all the desired information to the terminal
 * 
 * @param thePID takes in a certain pid from the findallpid function
 */
void getAllPIDparser(char *thePID)
{
    //local variables to be used for different flags
    char * myAllPidArray[4];
    char vMemAll[1000];
    char * myAllCmdArr[30];

    //allocating memory for arrays
    for(int i = 0; i < 4; i++)
    {
        myAllPidArray[i] = (char *)malloc(100);
    }

    for(int j = 0; j < 30; j++)
    {
        myAllCmdArr[j] = (char *)malloc(2000);
    }

    //printf("\nThe Inputted PID: %s\n", thePID);
    parseStat(thePID, myAllPidArray); //calling parseStat function to get desired information from stat function

    printf("PID: %s\t", myAllPidArray[0]);
    if (procState == true)
        printf("\tState: %s", myAllPidArray[1]);
    if (uTime == true)
        printf("\tUTime: %s", myAllPidArray[2]);
    if (sysTime == true)
        printf("\tSTime: %s", myAllPidArray[3]);
    if (vMemory == true)
    {
        getVMem(vMemAll, thePID); //calling getVMem function to get desired information from the statm file
        printf("\tVMem: %s", vMemAll);
    }
    if (dispCMD == true) //if displaycmd is true, we should also call the getCommandLine function and print the array from there
    {
        int c; 
        printf("\tCmdline: ");
        getCommandLine(myAllCmdArr, thePID); //calling getCommandLine function to get desired information from cmdline file
        for(c = 0;c<30;c++)
        {
            printf("%s ", myAllCmdArr[c]);	//print array 
        } 
    }
    printf("\n"); //adding a new line for nice formatting
}