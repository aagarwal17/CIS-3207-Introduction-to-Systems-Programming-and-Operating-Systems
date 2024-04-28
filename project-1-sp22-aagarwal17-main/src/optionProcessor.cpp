/**
 * @file optionProcessor.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief All of my Functions for dealing with commands inputted
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
#include <stdlib.h>

#include "parser.h"

/**
 * @brief checks if inputted pid is an actual pid
 * 
 * @param theNum the string of the potential pid characters
 * @return int return 0 if a valid pid and 1 if not
 */
int validPID(char *theNum)
{
    
    //Constructing the path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, theNum); 
    strcat(path, "/stat");

    //printf("\nThe path made: %s\n", path);
    if(access(path, F_OK ) == 0) 
    {
        //printf("\nSeems to be a valid PID\n");
        return 0;
    } 
    else 
    {
        printf("Not a valid PID\n");
        exit(EXIT_FAILURE);
    }
    return 1;

}

/**
 * @brief my function to parse the stat file. It reads through the file for the given pid and stores the values into an array of strings, which is referenced in the file that calls this function
 * 
 * @param inputPID the pid to parse stat
 * @param theArr the dereferenced array to be updated with stat information
 */
void parseStat(char * inputPID, char * theArr[])
{
    //Constructing a new path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, inputPID); 
    strcat(path, "/stat");


    FILE* inp; 
    inp = fopen(path,"r");		//filename of your data file 
    char arr[100][100];			//max string length 100 
    int i = 0; 
    while(1) //another way of saying while true
    { 
        //local variables
        char r = (char)fgetc(inp); 
        int k = 0;
        bool openParenFound = false; //boolean to check if an open parenthesis was found; needed because of spacing issue that may exist with this parameter in the file
        
        if (openParenFound == true && r == ' ')
        {
            continue;
        }
        while(r!=' ' && !feof(inp)) //read till a space or EOF 
        {
            if (r == '(')
            {
                openParenFound = true;
            }
            if (r == ')')
            {
                openParenFound = false;
            }

            arr[i][k++] = r;			//store in array 
            r = (char)fgetc(inp); 
        } 
        arr[i][k]=0;		//make last character of string null  
        if(feof(inp))   //check again for EOF 
        {		
            break; 
        } 
        i++; 
    }
    //Testing purposes: prints out values in array from stat file for given pid
    // int j; 
    // for(j = 0;j<=i;j++)
    // {
    //     printf("%s\n",arr[j] );	//print array 
    // } 

    //Copy the information from the local array into the array from the file that called this function, inputted as a parameter here
    //this allows us to access the array information in the other file
    strcpy(theArr[0], arr[0]);
    strcpy(theArr[1], arr[2]);
    strcpy(theArr[2], arr[13]);
    strcpy(theArr[3], arr[14]);
    
    // printf("%s\n", theArr[0]);
    // printf("%s\n", theArr[1]);
    // printf("%s\n", theArr[2]);
    // printf("%s\n", theArr[3]);
}

/**
 * @brief parses the statm file to get the virtual memory
 * 
 * @param virtMem string to hold the virtmem found in statfile
 * @param theNum the pid for the given case
 */
void getVMem(char *virtMem, char * theNum)
{
    //Constructing a new path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, theNum); 
    strcat(path, "/statm");

    //checking that the PID is valid
    if ((validPID(theNum) == 1))
    {
        printf("Not a valid PID");
        exit(EXIT_FAILURE);
    }

    //local variables to parse the statm file. We note that there are seven variables because there are seven fields in this file
    FILE* inp; 
    char two[50];
    char three[50];
    char four[50];
    char five[50];
    char six[50];
    char seven[50];

    inp = fopen(path, "r");

    fscanf(inp,"%s %s %s %s %s %s %s", virtMem, two, three, four, five, six, seven); //storing the text into the individual variables, and virtMem from the file who called this function is updated with the value found here
    //printf("string: %s", virtMem);
}

/**
 * @brief parse the command line to get the command line for the given pid
 * 
 * @param myCmdArr an array from the file that called this function to hold the pieces of the cmdline file being parsed
 * @param theNum the pid desired for this function call to get their cmdline file parsed
 */
void getCommandLine(char * myCmdArr[], char * theNum)
{
    
    //Constructing a new path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, theNum); 
    strcat(path, "/cmdline");

    //local variables
    FILE* inp; 
    inp = fopen(path,"r");		//filename of your data file

    int i = 0; 
    while(1) //another way of saying while true
    { 
        char r = (char)fgetc(inp); 
        int k = 0;
        
        
        while(r!='\0' && !feof(inp)) //read till a null character or EOF 
        {

            myCmdArr[i][k++] = r;			//store in array 
            r = (char)fgetc(inp); 
        } 
        myCmdArr[i][k]=0;		//make last character of string null  
        if(feof(inp))   //check again for EOF 
        {		
            break; 
        } 
        i++; 
    }
    //Testing purposes: a for loop to print all the elements of the array created
    // int j; 
    // for(j = 0;j<=i;j++)
    // {
    //     printf("%s\n",myCmdArr[j] );	//print array 
    // } 
}

/**
 * @brief When printing all pids of current user id to the terminal, we need to check if the pid from proc is a pid of the current user, which gets checked here
 * 
 * @param thePID the pid to check if it belongs to the current user
 * @return int return 0 if the pid belongs to the current user and return 1 if not
 */
int isUserId(char *thePID)
{
    //printf("The pid is %s\n", thePID);
    //Constructing a new path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, thePID); 
    strcat(path, "/status");
    //printf("The path is %s\n", path);

    //local variables
    FILE* inp; 
    inp = fopen(path,"r");		//filename of my data file 
    char arr[100][100];			//max word length 100
    int i = 0;
    //int tot = 0; //used for testing/printing purposes below

    if (!inp) //error checking for invalid inputted pid/path
    {
        perror("Failed to open file for pid");
        return 1;
    }
    
    //parsing the status file, while keeping note of end of line characters
    while(fgets(arr[i], 100, inp))
    {
        arr[i][strlen(arr[i]) - 1] = '\0';
        i++;
    }
    
    //Testing Purposes: printing the created array
    // tot = i;
    // for(i = 0; i < tot; ++i)
    // {
    //     printf(" %s\n", arr[i]);
    // }
    // printf("\n");
    //printf("Hopefully this is the right one: %s ", arr[8]);
    
    //If this pid's user id, stored in arr[8] is equal to the current user id, stated below, then we have the desired match, and we return 0
    if (strcmp(arr[8], "Uid:\t1000\t1000\t1000\t1000") == 0)
        return 0;
    else //otherwise, we do not have a match and return 1
        return 1;
    return 0; //if we make it here, we return 0 upon finish
}