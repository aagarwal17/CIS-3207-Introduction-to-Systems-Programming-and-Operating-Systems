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

bool thePID = false;
char *theProcessNumber;
bool processState = false;
bool userTime = true;
bool systemTime = false;
bool vMemUsed = false;
bool displaycmd = true;
bool processEntered = false;

void getAllPID();

int isPid(char *name)
{
    char *c;
    for (c = name; *c; c++)
    {
        if (!isdigit(*c))
        {
            return 0;
        }
    }
    return 1;
}

int parser(int argCount, char **args) 
{
    //strcpy(theProcessNumber,"1");
    int opt;
    //int PID;
    char * myArr[4];
    char vMem[10000];
    char * myCmdArr[50];

    //printf("In get opt:");
    // for ( int i = 0; i < 8; i++ )
    // {
    //     printf(" %s ", args[i]);
    // }

    for(int i = 0; i < 4; i++)
    {
        myArr[i] = (char *)malloc(100);
    }
    for(int j = 0; j < 50; j++)
    {
        myCmdArr[j] = (char *)malloc(2000);
    }  

    // put ':' in the starting of the string so that program can distinguish between '?' and ':' 
    while((opt = getopt(argCount, args, ":p:sUSvc")) != -1) 
    { 

        switch(opt) 
        { 
            case 'p':
                //thePID = true;
                printf("\nthe inputted PID: %s\n", optarg);
                processEntered = 1;

                theProcessNumber = optarg;
                if (isPid(optarg) == 0)
                {
                    //printf("\n Going to run get all PID func \n");
                    //getAllPID();
                }
                else if (validPID(theProcessNumber) == 0)
                {
                    parseStat(theProcessNumber, myArr);
                    printf("PID: %s", myArr[0]);
                }
                else
                {
                    perror("Not a valid PID");
                }
                break;
            case 's':
                processState = true;
                printf("\tState: %s", myArr[1]);
                break;
            case 'U':                            
                userTime = false;
                printf("\tUTime: %s", myArr[2]);
                break; 
            case 'S': 
                systemTime = true;
                printf("\tSTime: %s", myArr[3]);
                break; 
            case 'v':
                //vMemUsed = true;
                if (validPID(theProcessNumber) == 0)
                {
                    getVMem(vMem, theProcessNumber);
                    printf("\tVMem: %s", vMem);
                }
                else
                {
                    perror("In vMem: Not a valid PID");
                }
                break;
            case 'c':
                //displaycmd = false;
                if (validPID(theProcessNumber) == 0)
                {
                    getCommandLine(myCmdArr, myArr[0]);
                    int c; 
                    printf("\tCmdline: ");
                    for(c = 0;c<50;c++)
                    {
                        printf("%s ", myCmdArr[c]);	//print array 
                    } 
                    printf("\n");
                    //getCommandLine(displaycmd, myArr[0]);
                    //printf("\tVMem: %s\n", vMem);
                }
                else
                {
                    perror("In else: Not a valid PID");
                }
                break;
            case ':': 
                printf("Option needs a value\n"); 
                break; 
            case '?':
                if (optopt == 'p')
                    printf("Arg \"p\" needs a port specified. \n");
                else if (opt != '-') 
                    fprintf(stderr, "Unknown Option: '-%c'.\n", optopt);
                break; 
        } 
    }

    if (!processEntered)
    {
        getAllPID();
    }
    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argCount; optind++)
    {     
        printf("extra arguments: %s\n", args[optind]); 
    }
      
    return 0; 
}


void getAllPID()
{
    struct dirent *de;  // Pointer for directory entry
  
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir("/proc");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        //return 0;
    }
  
    const char *p;
    //char *myArgv;
    
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        p = de->d_name;

        if (isPid(de->d_name))
        { 

            char array[20][20] = {{0}};
            strcpy( array[0], "./tups");
            strcpy( array[1], "-p" );
            strcpy( array[2], p );
            strcpy( array[3], "-s" );
            strcpy( array[4], "-U" );
            strcpy( array[5], "-S" );
            strcpy( array[6], "-v" );
            strcpy( array[7], "-c" );

            char *ptr_array[8];
            for ( int i = 0; i < 8; i++ )
            {
                ptr_array[i] = array[i];
                printf("%s ", ptr_array[i]);
            }
            //char *myArgv[8] = {"./tups","-p", p, "-s", "-U", "-S", "-v", "-c"};     
            printf("\n");
            parser(8, ptr_array);
        }
    }
        
        
  
    closedir(dr);    
    //return 0;
}