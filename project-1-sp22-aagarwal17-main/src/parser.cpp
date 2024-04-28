/**
 * @file parser.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief My file to parse the command line if a -p is provided, ideally with a pid. It will also print out the desired information
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

/**
 * @brief Global variables to hold true/false values for parser/getopt function
 * 
 */
bool thePID = false;
char *theProcessNumber;
bool processState = false;
bool userTime = true;
bool systemTime = false;
bool vMemUsed = false;
bool displaycmd = true;

void getAllPID(); //this function exists below but needs to have his function header included at the top because the parser function calls this function

/**
 * @brief Checks if a given name, which should ideally be a potential pid, is a number
 * 
 * @param name a string that should ideally be a pid
 * @return int return 0 if name is found to not be a digit and return 1 if name is found to be a digit
 */
int isPid(char *name)
{
    char *c;
    for (c = name; *c; c++)
    {
        if (!isdigit(*c))
        {
            return 0;
        }
        else 
        {
            printf("Not a valid PID\n");
            //exit(EXIT_FAILURE);
        }
    }
    return 1;
}
/**
 * @brief My main parser function. Takes input from the main function to parse the command line and output the desired flag information
 * 
 * @param argCount number of args inputted into the terminal
 * @param args the array of strings inputted into the terminal
 * @return int return 0 upon finishing
 */
int parser(int argCount, char **args) 
{
    //local variables to be used for different flags
    int opt;
    char * myArr[4];
    char vMem[10000];
    char * myCmdArr[50];

    //allocating memory for arrays
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
            case 'p': //if -p inputted, set boolean thePID to true and print the inputted argument, optarg; also checks to make sure pid is valid
                thePID = true;
                printf("\nthe inputted PID/next arg inputted: %s\n", optarg);
                
                theProcessNumber = optarg;
                
                if (validPID(theProcessNumber) == 0)
                {
                    parseStat(theProcessNumber, myArr);
                }
                else
                {
                    perror("Not a valid PID\n");
                    //exit(EXIT_FAILURE);
                }
                break;
            case 's': //if -s inputted, set boolean processState to true
                processState = true;
                break;
            case 'U': //if -U inputted, set boolean userTime to false because we do not want to print this out if it included                            
                userTime = false;
                break; 
            case 'S': //if -S inputted, set boolean systemTime to true
                systemTime = true;
                break; 
            case 'v': //if -v inputted, set boolean vMemUsed to true; also, check to make sure pid is valid and find the vMemory
                vMemUsed = true;
                break;
            case 'c': //if -c inputted, set boolean displaycmd to false because we do not want to print out the cmdline if the flag is included
                displaycmd = false;
                break;
            case ':': //if -p is provided with no arg, print this statement
                printf("Option needs a value\n"); 
                break; 
            case '?': //if some invalid flag is inputted, print out the following error type messages
                if (optopt == 'p')
                    printf("Arg \"p\" needs a port specified. \n");
                else if (opt != '-') 
                    fprintf(stderr, "Unknown Option: '-%c'.\n", optopt);
                break; 
        } 
    }

    
    //This optind is for the extra arguments which are not parsed
    for(; optind < argCount; optind++)
    {     
        printf("extra arguments: %s\n", args[optind]); 
    }

    //Printing out the desired information if the boolean has a value of true:
    if (thePID == true)
        printf("PID: %s", myArr[0]);
    if (processState == true)
        printf("\tState: %s", myArr[1]);
    if (userTime == true)
        printf("\tUTime: %s", myArr[2]);
    if (systemTime == true)
        printf("\tSTime: %s", myArr[3]);
    if (vMemUsed == true)
    {
        if (validPID(theProcessNumber) == 0)
        {
            getVMem(vMem, theProcessNumber);
        }
        else
        {
            perror("In vMem: Not a valid PID\n");
        }
        printf("\tVMem: %s", vMem);
    }
    if (displaycmd == true) //if displaycmd is true, we should also call the getCommandLine function and print the array from there
    {
        int c; 
        printf("\tCmdline: ");
        getCommandLine(myCmdArr, theProcessNumber);
        for(c = 0;c<50;c++)
        {
            printf("%s ", myCmdArr[c]);	//print array 
        } 
    }
    printf("\n"); //adding a new line for nice formatting

    return 0; //return upon completion
}