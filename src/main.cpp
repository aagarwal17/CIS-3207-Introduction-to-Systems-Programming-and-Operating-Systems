/**
 * @file main.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief 
 * @version 0.1
 * @date 2022-02-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "parser.h"
#include "getProcessList.h"
#include <stdio.h>
#include <string.h>

int main (int argc, char **argv)
{
    // for ( int i = 0; i < 8; i++ )
    // {
    //     printf("%s ", argv[i]);
    // }

    if (strcmp(argv[1], "-p") != 0)
    {
        findAllPID();
    }
    else
    {
        //printf("Count of args: %d\n", argc);
        parser(argc, argv);
    }

    
    return 0;
}