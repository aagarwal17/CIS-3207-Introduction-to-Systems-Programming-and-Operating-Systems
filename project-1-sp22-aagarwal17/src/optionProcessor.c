/**
 * @file optionProcessor.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief Functions for dealing with commands inputted
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

// bool checkIfValidPID(char * inputPID)
// {
//     return 1;
// }

// // Helper function to check if a struct dirent from /proc is a PID folder.
// int is_pid_folder(const struct dirent *entry) 
// {
//     const char *p;

//     for (p = entry->d_name; *p; p++) 
//     {
//         if (!isdigit(*p))
//             return 0;
//     }

//     return 1;
// }

// char * parseStat(char * inputPID)
// {
//     char arrStatValues[3];
//     return " ";
// }

// char * parseStatm(char * inputPID)
// {
//     char arrStatmValues[2];
//     return 1;
// }

// char parseCmdline(char * inputPID)
// {
//     return 1;
// }

int validPID(char *theNum)
{
    
    //Constructing the path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, theNum); 
    strcat(path, "/stat");

    if(access(path, F_OK ) == 0) 
    {
        return 0;
    } 
    else 
    {
        return 1;
    }
}

void parseStat(char * inputPID, char * theArr[])
{
    //Constructing a new path for a file in the directory
    char path[1000];
    strcpy(path, "/proc/");
    strcat(path, inputPID); 
    strcat(path, "/stat");



    FILE* inp; 
    inp = fopen(path,"r");		//filename of your data file 
    char arr[100][100];			//max word length 50 
    int i = 0; 
    while(1)
    { 
        char r = (char)fgetc(inp); 
        int k = 0;
        bool openParenFound = false;
        
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

    
    // int j; 
    // for(j = 0;j<=i;j++)
    // {
    //     printf("%s\n",arr[j] );	//print array 
    // } 
    
    // char thePID[50] = (char) arr[0];
    // char theState[50] = arr[2];
    // char theUTime[50] = arr[13];
    // char theSTime[50] = arr[14];

    //char *arrStatValues[4];
    // strcpy(theArr[0], arr[0]);
    // printf("PID is %s\n", theArr[0]);
    // strcpy(theArr[1], arr[2]);
    // printf("PID is %s\n", theArr[1]);
    // strcpy(theArr[2], arr[13]);
    // strcpy(theArr[3], arr[14]);
    theArr[0] = arr[0];
    printf("%s", arr[0]);
    printf("PID is %s\n", theArr[0]);
    theArr[1] = arr[2];
    printf("%s", arr[2]);
    printf("PID is %s\n", theArr[2]);
    theArr[2] = arr[13];
    printf("PID is %s\n", theArr[13]);
    theArr[3] = arr[14];
    printf("PID is %s\n", theArr[14]);
    //return arrStatValues;
}


int getPID2(bool PIDState, char *theNum)
{
    if (PIDState == true)
    {
        printf("we made it this far \n");
    }
    
    
    return 0; 
 
}


// int getPID(bool PIDState, char * theNum)
// {
//     if (PIDState == true)
//     {
//         printf("we made it this far \n");
//     }

//     //DIR *procdir;
//     FILE *fp;
//     //struct dirent *entry;
//     char path[256 + 5 + 5]; // d_name + /proc + /stat

//     // Open /proc directory.
//     // procdir = opendir("/proc");
//     // if (!procdir) 
//     // {
//     //     perror("opendir failed");
//     //     return 1;
//     // }

//     // Iterate through all files and folders of /proc.
//     // while ((entry = readdir(procdir))) 
//     // {
//     //     // Skip anything that is not a PID folder.
//     //     if (!is_pid_folder(entry))
//     //         continue;

//         // Try to open /proc/<PID>/stat.
//         snprintf(path, sizeof(path), "/proc/%s/stat", theNum);
//         fp = fopen(path, "r");

//         if (!fp) 
//         {
//             perror(path);
//             //continue;
//         }


//         char str[10000];
//         char arrayOfLines[49][100];
//         const char s[2] = " ";
//         char *token;
//         int i = 0;
        
//         while(fgets(str, sizeof(str) / sizeof(str[0]), fp) != NULL)
//         {
//             char *ptr1 = strtok(str, " ");
//             strcpy(arrayOfLines[i], ptr1);
//             i++;
//         }
        
//         for (int i = 0; i < 101; i++) 
//         {
//             printf("%s\n", arrayOfLines[i]);
//         }        

//         // /* get the first token */
//         // token = strtok(str, s);
        
//         // /* walk through other tokens */
//         // while( token != NULL ) 
//         // {
//         //     printf( " %s\n", token );
            
//         //     token = strtok(NULL, s);
//         // }

//     //}

//     return 0;
// }

// char getStateInfo(bool StateInfo, char * theNum)
// {
//     return 1;
// }

// char getUserTime(bool UTimeInfo, char * theNum)
// {
//     return 1;
// }

// char getSysTime(bool SysTimeInfo, char * theNum)
// {
//     return 1;
// }

// char getVMem(bool VMemInfo, char * theNum)
// {
//     return 1;
// }

// char getCommandLine(bool cmdInfo, char * theNum)
// {
//     return 1; 
// }