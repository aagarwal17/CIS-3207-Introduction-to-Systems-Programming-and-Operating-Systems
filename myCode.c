/**
 * @file tuls.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief  tuls lists information about the FILES in a directory. By default, tuls lists the files in the current directory and will list the files in a specified directory, if the argument is passed.
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief listAllFiles is my main function to print out the names of all files from the given directory
 * 
 * @param inputtedPath as the name suggests, this is the provided directory path, which is used to then find and print all files within that directory
 */
void listAllFiles(char *inputtedPath, int depth)
{
    DIR *dir; //Pointer to directory stream
    struct dirent *dirEntry; //Pointer to a structure dirent type representing a directory entry

    //opendir() opens a directory stream corresponding to the file path pointed by dir (the directory stream)
    //By default, the opened directory stream points to the first entry in the directy
    //The function returns a pointer to dir (the directory stream); otherwise, it returns NULL. Thus, I run this statement if it does not equal NULL
    if ((dir = opendir(inputtedPath)) != NULL) 
    {
        //The readdir() function returns a file/directory at the next position in the directory stream, pointed to by dir, on every call
        //readdir() function returns NULL at the end of the directory stream, so we have the while loop run until we hit NULL
        //Thus, while the next directory entry in the directory stream does not equal null, print that directory entry
        //Prints all the files and subfiles within a directory
        while ((dirEntry = readdir(dir)) != NULL) 
        {
            // struct stat path_stat; //creating path_stat to be used in stat function

            if ((strcmp(dirEntry->d_name, ".") != 0) && (strcmp(dirEntry->d_name, "..") != 0))
            {
                if (depth > 0)
                {
                    printf ("%*s->%s\n", depth*5, " ", dirEntry->d_name); //the -> is used to specify a component of the structure of dirEntry. In this case, one that has the entry name, so we print the current directory stream item name
                }
                else
                {
                    printf ("%s\n", dirEntry->d_name);
                }
                
                // stat(dirEntry->d_name, &path_stat); //stat function is used for checking if the directory entry is a file or folder/directory. It obtains information about the named file and writes it to the area pointed to by the buf argument


                //S_ISDIR is a macro to check whether the stat struct is a directory
                // if (S_ISDIR(path_stat.st_mode)) //st_mode is the field that contains the file type and mode
                // {
                //     // Construct new path from our base path
                //     char path[1000];
                //     strcpy(path, inputtedPath); //copying the original inputted path into our created path
                //     strcat(path, "/");  //adding / to the end of our path to make it work in our function in the recursive step
                //     strcat(path, dirEntry->d_name); //finally, adding in the current directory path to our path
                //     listAllFiles(path, (depth+1));     //finally, calling the function on this path to be able to get the files within this directory stream
                // }
                
                if (dirEntry->d_type == DT_DIR)
                {
                    printf("%*s[%s]\n", (depth+1)*5, "", dirEntry->d_name);
                    // Construct new path from our base path
                    char path[1000];
                    strcpy(path, inputtedPath); //copying the original inputted path into our created path
                    strcat(path, "/");  //adding / to the end of our path to make it work in our function in the recursive step
                    strcat(path, dirEntry->d_name); //finally, adding in the current directory path to our path
                    listAllFiles(path, depth+1);     //finally, calling the function on this path to be able to get the files within this directory stream
                }
                
            }
        }
        //closerdir() function closes a directory pointed by dir; returns 0 on success and -1 on error
        closedir (dir); //if we get to this line, then it will return 0, indicating success
    } 
    //If unable to open the specificed directory stream..
    else 
    {
        //If the directory could not be opened, we print the following message:
        printf("tuls: cannot open directory\n");         //Do I need to change this from a perror statement to a normal printf statement?
        exit(EXIT_FAILURE); //EXIT_FAILURE is equivalent to status code 1
    }
}

/**
 * @brief main function for passing information, specifically the paths, to a program at execution time
 * 
 * @param argc the argument count, or the number of arguments passed to the program
 * @param argv the argument vector/values, the array which holds the arguments passed into the function as strings
 * @return int return 0 upon success and 1 upon failure (or EXIT_FAILURE)
 */
int main(int argc, char** argv)
{

    printf("Current Working Directory: \n./\n");
    if (argc == 1)
    {
        listAllFiles(".", 0);
        return 0; //Returning 0 indicates success
    }
    else if (argc == 2)
    {
        listAllFiles(".", 0);

        printf("\nDirectory Inputted: \n%s\n", argv[1]);

        char *inputPath = (char *) malloc(strlen(argv[1]));
        
        //If the last character is a '/':
        if (argv[1][strlen(argv[1]) - 1] == '/')
        {
            strncpy(inputPath, argv[1], strlen(argv[1]) - 1);
        }
        else
        {
            strcpy(inputPath, argv[1]);
        }
        listAllFiles(inputPath, 0);
        return 0; //Returning 0 indicates success
    }
    //As stated by TA Alexander, if there are more than two args passed, we should just print out the current directory
    else if (argc > 2)
    {
        listAllFiles(".", 0);
        return 0; //Returning 0 indicates success
    }
    else
    {
        //Do I need to change this from a perror statement to a normal printf statement?
        printf("tuls: cannot open directory\n");
        exit(EXIT_FAILURE); //EXIT_FAILURE is equivalent to status code 1
    }
}
