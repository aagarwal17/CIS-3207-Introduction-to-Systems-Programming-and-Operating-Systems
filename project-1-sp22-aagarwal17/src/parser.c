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

bool thePID = false;
char *theProcessNumber;
bool processState = false;
bool userTime = true;
bool systemTime = false;
bool vMemUsed = false;
bool displaycmd = true;


int parser(int argCount, char **args) 
{
    int opt;
    //int PID;
    char * myArr[4]; 

    // put ':' in the starting of the string so that program can distinguish between '?' and ':' 
    while((opt = getopt(argCount, args, ":p:sUSvc")) != -1) 
    { 
        switch(opt) 
        { 
            case 'p':
                //thePID = true;
                printf("the inputted PID: %s\n", optarg); 
                theProcessNumber = optarg;
                if (validPID(theProcessNumber) == 0)
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
                printf("test U\n");
                userTime = false; 
            case 'S': 
                printf("test S\n");                
                systemTime = true;
                break; 
            case 'v':
                printf("test v\n");
                vMemUsed = true;
            case 'c':
                printf("test c\n");
                displaycmd = false;
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
    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argCount; optind++)
    {     
        printf("extra arguments: %s\n", args[optind]); 
    }
      
    return 0; 
}    