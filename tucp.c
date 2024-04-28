/**
 * @file trial.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief tucp basically copy files or a group of files to a certain location
 * @version 0.1
 * @date 2022-01-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief Function to determine if a given path name references a directory
 * 
 * @param path_name the path that potentially points to a directory
 * @return int Return 1 if path_name represents a directory and 0 if it does not or on failure
 */
int is_Dir(char *path_name)
{
    struct stat buff; //holds information about objects in a file system

    if (stat(path_name, &buff) < 0)
    {
        fprintf(stderr, "stat: %s\n", strerror(errno));
        return 0;
    }
    return S_ISDIR(buff.st_mode);
}

/**
 * @brief copyFile is my main function to do the copying procedure
 * 
 * @param sourceFile as the name suggests, this is the path of the source file to get copied
 * @param destinationFile as the name suggests, this is the path of the destination file
 */
void copyFile(char *sourceFile, char *destinationFile)
{
    //Opening the source file in read mode
    FILE *fp = fopen(sourceFile, "r");

    FILE *fptw = fopen(destinationFile, "w+");

    int character; //character holds a character in a file

    //If the file is opened without any error:
    if(fp != NULL)
    {
        //Reading character by character and putting it in destination file
        while ((character = fgetc(fp)) != EOF)
        {
            fputc(character, fptw);
        }
        printf("      Copy Successful!\n");
        fclose(fp);
        fclose(fptw);
    }
    else
    {
        printf("     Could not open inputted file\n");
        //exit(EXIT_FAILURE); //indicates failure
    }
}



/**
 * @brief main function for passing information, specifically the files/directories, to a program at execution time
 * 
 * @param argc the argument count, or the number of arguments passed to the program
 * @param argv the argument vector/values, the array which holds the arguments passed into the function as strings
 * @return int return 0 upon success and 1 upon failure (or EXIT_FAILURE)
 */
int main(int argc, char** argv)
{

    if (argc < 3)
    {
        printf("Error: not enough arguments\n");
        exit(EXIT_FAILURE); //EXIT_FAILURE is equivalent to status code 1
    }
    else if (argc == 3)
    {

        if (is_Dir(argv[1]) == 0 && is_Dir(argv[2]) == 0)
        {
            printf("File to Copy: %s\n", argv[1]);
            printf("File to Create/Overwrite: %s\n", argv[2]);
            //call our function to copy given file to destination file
            copyFile(argv[1],argv[2]);
            return 0; //Returning 0 indicates success
        }
        else if (is_Dir(argv[1]) == 0 && is_Dir(argv[2]) == 1)
        {
            //we need to copy given file into a directory
            printf("File to Copy: %s\n", argv[1]);
            printf("Destination Directory for Input File: %s\n", argv[2]);

            char *destDir = (char *) malloc(strlen(argv[2]));
            //If the last character is a '/':
            if (argv[2][strlen(argv[2]) - 1] == '/')
            {
                strncpy(destDir, argv[2], strlen(argv[2]) - 1);
            }
            else
            {
                strcpy(destDir, argv[2]);
            }
            
            //Constructing a new path for a file in the directory
            char path[1000];
            strcpy(path, destDir); //copying the original inputted path into our created path
            strcat(path, "/");  //adding / to the end of our path to make it work in our function (a complete path)
            strcat(path, argv[1]); //finally, adding in the current file to the end of our path
            copyFile(argv[1], path);     //finally, calling the function on this path to be able to make the appropiate copy
                
            return 0; //Returning 0 indicates success
        }
        else
        {
            printf("Error: Wrong order of file and directory\n");
            exit(1); //EXIT_FAILURE is equivalent to status code 1
        }

    }
    else if (argc > 3)
    {
        if (is_Dir(argv[argc - 1]) == 0)
        {
            printf("Error: More than three arguments inputted but final argument is not a directory\n");
            exit(EXIT_FAILURE); //EXIT_FAILURE is equivalent to status code 1
        }
        else
        {
            //Last argument is a directory, and we have multiple files to put in that directory
            int i;
            for (i = 1; i < argc - 1; i++)
            {
                //char currentFile = argv[i];
                printf("File %d to Copy: %s", i, argv[i]);
                
                char *destDir = (char *) malloc(strlen(argv[argc - 1]));
                //If the last character is a '/':
                if (argv[argc - 1][strlen(argv[2]) - 1] == '/')
                {
                    strncpy(destDir, argv[argc - 1], strlen(argv[argc - 1]) - 1);
                }
                else
                {
                    strcpy(destDir, argv[argc - 1]);
                }
                
                //Constructing a new path for a file in the directory
                char path[1000];
                strcpy(path, destDir); //copying the original inputted path into our created path
                strcat(path, "/");  //adding / to the end of our path to make it work in our function (a complete path)
                strcat(path, argv[i]); //finally, adding in the current file to the end of our path
                copyFile(argv[i], path);     //finally, calling the function on this path to be able to make the appropiate copy
                    
            }
            printf("Destination Directory for Input Files: %s\n", argv[argc - 1]);
            return 0; //Returning 0 indicates success            
        }

    }
    else
    {
        printf("Error:");
        exit(EXIT_FAILURE); //EXIT_FAILURE is equivalent to status code 1
    }
}
//End of Code :)    