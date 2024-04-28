/**
 * @file main.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief main file to allow the program to run as intended
 * @version 0.1
 * @date 2022-02-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "parser.h" //Include the header file for parser.c to access the parser function
#include "getProcessList.h" //Include the header file for getProcessList.c to access findAllPID function
#include <stdio.h>
#include <string.h>
/**
 * @brief Main function which allows the program to run
 * 
 * @param argc Number of arguments inputted into the terminal
 * @param argv Array from terminal
 * @return int 
 */
int main (int argc, char **argv)
{
    int foundP = 0;
    /**
     * @brief Find if the user entered -p as any arg. If found, increment variable foundP to 1, indicating the need to use normal parser. Else, use all pid function
     * 
     */
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            foundP = 1;
        }
    }
    if (foundP == 0)
        allParser(argc,argv); //allParser function finds all pid information for current user; function exists in getProcesslist.c
    else
    {
        parser(argc, argv); //parser function finds all pid information for given pid; function exists in parser.c
    }    
    return 0; //return success
}