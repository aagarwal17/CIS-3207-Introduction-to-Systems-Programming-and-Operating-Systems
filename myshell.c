/**
 * @file myshell.c
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief This is my main and only c file that creates my shell for Assignment 2 of CIS 3207. See functions for details
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//Libraries I have used for this assignment
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//we set a max command line of 256 characters
#define MAX_LENGTH 256

// Function headers written at the top so that I can easily access functions throughout the program without worrying about order
void promptUser();
void process(char *line);
char **parse(char *line);
int executeNormally(char **command, int builtinNo);
int executeBackground(char **command, int builtinNo);
int executeRedirection(char **command, char *inputName, char *outputName, int isAppend, int builtinNo);
int executePipe(char **command1, char **command2, int builtinNo1, int builtinNo2);
int checkBuiltins(char *args);
int exec_cd(char **args);
int exec_clr(char **args);
int exec_dir(char **args);
int exec_environ(char **args);
int exec_echo(char **args);
int exec_help(char **args);
int exec_pause(char **args);

// list of built-in handler functions
int (*builtins[])(char **) = {
    &exec_cd,
    &exec_clr,
    &exec_dir,
    &exec_environ,
    &exec_echo,
    &exec_help,
    &exec_pause};

// list of built-in commands
char *builtin_cmd[] = {
    "cd",
    "clr",
    "dir",
    "environ",
    "echo",
    "help",
    "pause"};

// for readme file
char originalDir[MAX_LENGTH];

//Array to hold environment variables from main
char * myEnvironArr[100];

/**
 * @brief Main function that starts my shell
 * 
 * @param args args inputted before starting the shell (used to determine if in batch mode or interactive mode)
 * @param argv array of above args
 * @param envp contains environment variables and is used for environ built-in function later
 * @return int 
 */
int main(int args, char *argv[], char *envp[])
{
    getcwd(originalDir, sizeof(originalDir)); //gets the current working directory to be used for the prompt


    //CODE FOR GETTING ENVIRONMENT VARIABLES:
    //allocating memory for arrays
    for(int j = 0; j < 100; j++)
    {
        myEnvironArr[j] = (char *)malloc(5000);
    }

    char* ptr = 0; //helper variable to store what strtok returns
    int j = 0;    

    // while the current string pointed to by *env_variable is not empty, increment it.
    while(*envp)
    {
        int i = 0; /* this is a control variable that is used to separate the left from the right part (divided by '=') */

        char temp[255] = ""; /* temporary string that will hold the whole current environment variable */

        strcpy(temp, *envp++);

        ptr = strtok(temp, "="); /* we specify the delimiters here, 'ptr' first points to the left part */

        char temp_a[255]; /* these two strings will hold the left and the right part respectively*/
        char temp_b[1000];

        while (ptr != NULL) 
        {            
            if (i == 0) /* in the first iteration we get the left part so we store it */
                strcpy(temp_a, ptr);

            if (i == 1) /* in the second iteration we get the right part so we store it */
            {    
                strcpy(temp_b, ptr);
            }
            ptr = strtok(NULL, "="); /* set 'ptr' to point to the right part, if this is already the second iteration, it will point to NULL */

            i++;
        }

    strcpy(myEnvironArr[j], temp_a);
    //printf("My Environ Arr: %s\n", myEnvironArr[j]);
    j++;    
    }


    //Checking if we are in batch mode or interactive mode
    if (argv[1] != NULL) // if reading from a file, then execute steps for batch mode
    { 
        FILE *fp;
        if ((fp = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Batchfile cannot be opened with name %s.\n", argv[1]);
            return 0;
        }
        char line[MAX_LENGTH];
        // gets lines
        while (fgets(line, MAX_LENGTH, fp))
        {
            printf("line: %s", line);
            // replaces new line character with string ending character.
            line[strlen(line) - 1] = '\0';
            if (strcmp(line, "quit") == 0)
            { // quits the shell.
                exit(0);
            }
            process(line);
        }
        fclose(fp);
    }
    else //Otherwise, we are in interactive mode
    {
        // Interactive mode
        char line[MAX_LENGTH];
        while (1)
        {
            promptUser(); // Prints prompt.
            fgets(line, MAX_LENGTH, stdin);
            line[strlen(line) - 1] = '\0';
            process(line); // processes the command line
        }
    }
    return 0;
}

void promptUser()
{
    // Prints prompt.
    char currentDir[MAX_LENGTH];
    getcwd(currentDir, sizeof(currentDir));
    fprintf(stdout, "%s>", currentDir);
}
/**
 * @brief my main function for parsing and understanding the inputted command from the user
 * 
 * @param line the line/command inputted into the terminal/shell
 */
void process(char *line)
{
    // Parses command line to array of arguments
    char **args = parse(line);
    if (args[0] == NULL)
    {
        return;
    }
    // Booleans to switch between functions
    int isBg = 0;
    int isPipe = 0;
    int hasInput = 0;
    int hasOutput = 0;
    int isAppend = 0; // Need this to differentiate between ">" and ">>"

    // In myshell, we execute 1 command/program at a time normally, or 2 commands/programs when invoking a pipe.
    //Thus, command2 is only needed for a pipe
    char *command1[MAX_LENGTH];
    char *command2[MAX_LENGTH]; // Only available when invoking a pipe
    
    // initializing to catch null cases
    command1[0] = NULL;
    command2[0] = NULL;
    // declaring and initializing the input and output file name. Will do a strcpy() if redirection is found in command line
    char inputName[MAX_LENGTH];
    char outputName[MAX_LENGTH];
    inputName[0] = '\0';
    outputName[0] = '\0';

    //Code to start processing line inputted into the terminal
    int i = 0;
    for (; args[i] != NULL; i++)
    {
        //If we find the ampersand sign, immediately invoke executeBg(). Contents of the command line after the ampersand sign will be ignored for simplicity
        //The Instructions say that this is allowed
        if (strcmp(args[i], "&") == 0)
        {
            isBg = 1;
            break;
        }

        //If we find the pipe sign, stop putting arguments into command 1 and start putting arguments into command 2
        if (strcmp(args[i], "|") == 0)
        {
            isPipe = 1;
            command1[i] = NULL;
            i++;
            int j = 0;
            while (args[i] != NULL)
            {
                command2[j] = args[i];
                i++;
                j++;
            }
            command2[j] = NULL;
            break;
        }
        //If we find the redirection flag (no appending), check for number of arguments and validity of file name; then copy string into stated file
        if (strcmp(args[i], ">") == 0)
        {
            if (hasOutput)
            {
                printf("%s\n", "myshell works with only one output file at a time.");
                return;
            }
            hasOutput = 1;
            command1[i] = NULL;
            // Checks if file name is given
            if (args[++i] == NULL)
            {
                fprintf(stderr, "%s\n", "Cannot execute redirection: Output file name is not given.");
                return;
            }
            else
            {
                // Set input/output file name
                strcpy(outputName, args[i]);
            }
        }
        
        //If we find the redirection flag (with appending), check for number of arguments and validity of file name; then copy string into stated file
        else if (strcmp(args[i], ">>") == 0)
        {
            if (hasOutput)
            {
                printf("%s\n", "myshell works with only one output file at a time.");
                return;
            }
            hasOutput = 1;
            command1[i] = NULL;
            if (args[++i] == NULL)
            {
                fprintf(stderr, "%s\n", "Cannot execute redirection: Output file name is not given.");
                return;
            }
            else
            {
                strcpy(outputName, args[i]);
                isAppend = 1;
            }
        }

        //If we find the redirection flag (with appending) in other direction, check for number of arguments and validity of file name; then copy args into file
        else if (strcmp(args[i], "<") == 0)
        {
            if (hasInput)
            {
                printf("%s\n", "myshell works with only one output file at a time.");
                return;
            }
            hasInput = 1;
            command1[i] = NULL;
            if (args[++i] == NULL)
            {
                fprintf(stderr, "%s\n", "Cannot execute redirection: Input file name is not given.");
                return;
            }
            else
            {
                strcpy(inputName, args[i]);
            }
        }
        //For other cases
        else
        {
            //If the function has caught any redirection, passing more arguments or commands will cause an error and we should output this
            //Exception case: commands > file1 < file2
            if (hasInput | hasOutput)
            {
                fprintf(stderr, "%s\n", "Too many argument or ambiguous command.");
                return;
            }
            else
            {
                command1[i] = args[i];
            }
        }
    }

    //If we are not in a pipe and we found no input and no output, set this command to null
    if ((!isPipe) && (!hasInput) && (!hasOutput))
    {
        command1[i] = NULL;
    }

    //If command1[0] is null, we have a missing or ambiguous command
    if (command1[0] == NULL)
    {
        fprintf(stderr, "%s\n", "Missing or ambiguous command");
        return;
    }
    //Checking if the command is a built in command and setting the return value to builtinNo1
    //We use builtinNo to decide which functions to call (or program execution) later on
    //whether the command is a built in or not, we still have a number returned, which we use in our other functions to determine which functions to subsequently call.
    int builtinNo1 = checkBuiltins(command1[0]);
    
    //If it is a pipe:
    if (isPipe)
    {
        if (command2[0] == NULL)
        {
            fprintf(stderr, "%s\n", "Cannot execute pipe: missing or ambiguous command 2");
        }
        else
        {
            int builtinNo2 = checkBuiltins(command2[0]);
            executePipe(command1, command2, builtinNo1, builtinNo2);
        }
        return;
    }

    //If it is a background process:
    else if (isBg)
    {
        executeBackground(command1, builtinNo1); //calling our function to deal with execution in background
        return;
    }

    //if it is redirection case:
    else if (hasInput | hasOutput)
    {

        executeRedirection(command1, inputName, outputName, isAppend, builtinNo1); //calling our function to deal with redirection
        return;
    }

    //Otherwise execute in the normal fashion
    else
    {
        executeNormally(command1, builtinNo1);
    }

    //Prof said we should call free after every line run. Also makes sense since we need to reset our args for a different line
    free(args);
    return;
}

/**
 * @brief our function to parse the command line
 * 
 * @param line the line to tokenize
 * @return char** returns the double array of strings from the inputted line
 */
char **parse(char *line)
{
    char **args; //creating a local variable to store each of the strings in the input line
    char *arg; //a single arg inputted

    if ((args = malloc(sizeof(char *) * MAX_LENGTH)) == NULL)
    {
        fprintf(stderr, "%s\n", "Error in parsing command line");
        exit(EXIT_FAILURE);
    }

    //As reccomended by TA, we tokenize the line using strtok and pass the arguments to the array of strings
    arg = strtok(line, " \t\r\n\a");
    int pos = 0;
    while (arg != NULL)
    {
        args[pos++] = arg;
        arg = strtok(NULL, " \t\r\n\a");
    }

    args[pos] = NULL;
    return args; //output/return the tokenized line
}

/**
 * @brief This function is soley for normal execution, either executing a program or running built-in commands
 * 
 * @param command the command to be executed
 * @param builtinNo a number which indicates whether this is a built in command or not
 * @return int 
 */
int executeNormally(char **command, int builtinNo)
{
    //if we have a built in:
    if (builtinNo != -1)
    {
        //try and run the built in command function
        if ((*builtins[builtinNo])(command) != 0)
        {
            fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo]);
            return -1;
        }
        return 0;
    }

    //Otherwise, we do not have a built-in function, so we should fork and exec for this external command
    // Program invocation:
    pid_t pid;
    if ((pid = fork()) == -1) //error checking
    {
        fprintf(stderr, "Fork error.\n");
        return -1;
    }
    //if child
    else if (pid == 0)
    {
        if (execvp(command[0], command) != 0)
        {
            fprintf(stderr, "Cannot start the program.\n");
        }
        exit(0);
    }
    //if parent, wait
    else
    {
        wait(NULL);
    }
    return 0;
}

/**
 * @brief function for executing in the background
 * 
 * @param command the command to be executed
 * @param builtinNo a number which indicates whether this is a built in command or not
 * @return int 
 */
int executeBackground(char **command, int builtinNo)
{
    //if we are dealing with a built in:
    if (builtinNo != -1)
    {
        if ((*builtins[builtinNo])(command) != 0)
        {
            fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo]);
            return -1;
        }
        return 0;
    }
    //Otherwise, we fork and exec
    pid_t pid;
    if ((pid = fork()) == -1)
    {
        fprintf(stderr, "Fork error.\n");
        return -1;
    }
    else if (pid == 0)
    {
        if (execvp(command[0], command) != 0)
        {
            fprintf(stderr, "Cannot start the program .\n");
        }
        exit(0);
    }
    //NOTE: WE DO NOT CALL WAIT
    return 0;
}

/**
 * @brief function for executing redirection for the given command
 * 
 * @param command command provided
 * @param inputName //name of input file
 * @param outputName //name of output file
 * @param isAppend //boolean for checking if we should be appending
 * @param builtinNo a number which indicates whether this is a built in command or not
 * @return int 
 */
int executeRedirection(char **command, char *inputName, char *outputName, int isAppend, int builtinNo)
{
    pid_t pid;
    if ((pid = fork()) == -1) //checking for forking error
    {
        fprintf(stderr, "Fork error.\n");
        return -1;
    }
    //if child:
    else if (pid == 0)
    {
        int fdIn;
        int fdOut;
        if (inputName[0] != '\0') //if we are given an input file, try to open it and change where it is pointing to
        {
            if ((fdIn = open(inputName, O_RDONLY, 0777)) == -1)
            {
                fprintf(stderr, "%s\n", "Cannot open input file.");
                exit(1);
            }
            dup2(fdIn, 0);
        }
        close(fdIn); //we should always remember to close

        //if we are given an output file, try to open it and change where it is pointing to
        if (outputName[0] != '\0')
        {
            if (isAppend) //check the same case if we are or are not appending for error:
            {
                if ((fdOut = open(outputName, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
                {
                    fprintf(stderr, "%s\n", "Cannot process. Please check for invalid file name.");
                    exit(1);
                }
            }
            else
            {
                if ((fdOut = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
                {
                    fprintf(stderr, "%s\n", "Cannot process. Please check for invalid file name.");
                    exit(1);
                }
            }
            dup2(fdOut, 1);
        }
        close(fdOut);

        //if we have a built in, try to run the corresponding function for the built in or print error
        if (builtinNo != -1)
        {
            if ((*builtins[builtinNo])(command) != 0)
            {
                fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo]);
                exit(1);
            }
            exit(0);
        }

        //run the command and output an error if necessary
        if (execvp(command[0], command) != 0)
        {
            fprintf(stderr, "Cannot start the program.\n");
            exit(1);
        }
    }
    //if we are the parent, wait
    else
    {
        wait(NULL);
    }
    return 0;
}

/**
 * @brief function to handle the case that we have a pipe
 * 
 * @param command1 first command in pipe to execute
 * @param command2 second command in pipe to execute
 * @param builtinNo1 a number which indicates whether this is a built in command or not
 * @param builtinNo2 a number which indicates whether this is a built in command or not
 * @return int 
 */
int executePipe(char **command1, char **command2, int builtinNo1, int builtinNo2)
{
    //This is my first attempt at piping. It seems to work but I was getting a small error message. I wanted to keep the code just in case I needed it.

    /*
    int fd[2];
    pid_t pid;
    pipe(fd);
    if ((pid = fork()) == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        if (builtinNo1 != -1)
        {
            if ((*builtins[builtinNo1])(command1) != 0)
            {
                fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo1]);
                return -1;
            }
            exit(0);
        }
        if (execvp(command1[0], command1) != 0)
        {
            fprintf(stderr, "Cannot start the program.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if ((pid = fork()) == 0)
        {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            if (builtinNo2 != -1)
            {
                if ((*builtins[builtinNo2])(command2) != 0)
                {
                    fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo2]);
                    return -1;
                }
                exit(0);
            }
            if (execvp(command2[0], command2) != 0)
            {
                fprintf(stderr, "Cannot start the second program.\n");
                exit(1);
            }
        }
        close(fd[0]);
    }
    wait(NULL);
    close(fd[0]);
    close(fd[1]);
    return 0;
    */

    int fd[2];
    pid_t pid1; //pid for process 1
    pid_t pid2; //pid for process 2

    pipe(fd);

    if ((pid1 = fork()) == -1) //checking for fork error
    {
        perror("Error in Fork 1");
        return 1;
    }

    //For process 1:
    if (pid1 == 0)
    {
        dup2(fd[1],1); //copy what is in fd[WRITE] TO STDOUT_FILENO
        
        //Closing read end
        close(fd[0]); //close read end of pipe

        //Executing program to the left of the pipe:
        if (builtinNo1 != -1)
        {
            if ((*builtins[builtinNo1])(command1) != 0)
            {
                fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo1]);
                return -1;
            }
            exit(0);
        }
        //if not a built in, we call execvp:
        else if (execvp(command1[0], command1) != 0)
        {
            fprintf(stderr, "Cannot start the program.\n");
            exit(EXIT_FAILURE);
        }
        exit(0);
    }
    // else if (pid1 != 0)
    // {
    //     wait(NULL);
    // }

    //for second process, we fork and check for errors
    if ((pid2 = fork()) == -1)
    {
        perror("Error in Fork 2");
        return 1;
    }

    //For process 2:
    if (pid2 == 0)
    {
        dup2(fd[0], 0); //copy what is in fd[READ] into STDIN_FILENO

        //Closing write end
        close(fd[1]); //closing write end of pipe

        //Executing program to the right of the pipe:
        if (builtinNo2 != -1)
            {
                if ((*builtins[builtinNo2])(command2) != 0)
                {
                    fprintf(stderr, "%s %s\n", "Failed to execute built-in command:", builtin_cmd[builtinNo2]);
                    return -1;
                }
                exit(0);
            }
        //if not a built in command, we call execvp for the external command:
        else if (execvp(command2[0], command2) != 0)
        {
            fprintf(stderr, "Cannot start the second program.\n");
            exit(1);
        }
        exit(0);
    }
    //otherwise, if parent, wait
    else if (pid2 != 0)
    {
        wait(NULL);
    }
    
    //Closing fd read
    close(fd[0]);

    //Closing fd write
    close(fd[1]);

    return 1;
}

/**
 * @brief my function to check if a command if one of our internal/built-in commands
 * 
 * @param args contains the command(s)
 * @return int return a number from our struct of builtin_cmd if it is a built_in command and return -1 if not a built-in command
 */
int checkBuiltins(char *args)
{
    //special case for quit to make life easier
    if (strcmp(args, "quit") == 0)
    {
        exit(0);
    }
    int i = 0;
    for (; i < 7; i++)
    { // number of builtin command with quit
        if (strcmp(args, builtin_cmd[i]) == 0)
        {
            return i;
        }
    }
    return -1; //if we get here, it is not a built in command, so return -1
}

/**
 * @brief my function for changing the directory
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int success state
 */
int exec_cd(char **args)
{
    //if user put a third argument, return error
    if (args[2] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }
    //if no second argument, print current working directory
    if (args[1] == NULL)
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) 
        {
            printf("Current working dir: %s\n", cwd);
        } 
        else 
        {
            perror("getcwd() error");
            return 1;
        }        
    }
    //otherwise, we should chdir() to the directory inputted and output error message if needed
    else
    {
        if (chdir(args[1]) == -1)
        {
            fprintf(stderr, "cd: %s: %s\n", strerror(errno), args[1]);
            return -1;
        }
    }
    return 0; //if we get here, we should return 0 to indicate success
}

/**
 * @brief my function for clearing the screen
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int the success state
 */
int exec_clr(char **args)
{
    //we should not have a second argument, so output an error message if we do
    if (args[1] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }
    printf("\e[1;H\e[2J"); //this was provided by TA Alexander and reccomended to be used rather than outputting a bunch of empty lines
    return 0;
}

/**
 * @brief built in function for reading the directory inputted or current directory
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int returns success state
 */
int exec_dir(char **args)
{
    //if a third argument is provided, print error message
    if (args[2] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }
    //creating local variables needed to print out given directory
    DIR *dp;
    struct dirent *dir;
    char path[MAX_LENGTH];

    //if no second argument provided, we should print out the contents of the current directory
    if (args[1] == NULL)
    {
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) 
        {
            while ((dir = readdir(d)) != NULL) 
            {
                printf("%s\n", dir->d_name);
            }
            closedir(d);
        }
        return(0);
        //fprintf(stderr, "Error when executing dir: no path provided.\n");
        //return -1;
    }

    //Otherwise, we print out the contents of the desired directory and print out error messages where needed
    else
    {
        strcpy(path, args[1]);
        if ((dp = opendir(path)) == NULL)
        {
            fprintf(stderr, "Opendir %s error", path);
        }
        while ((dir = readdir(dp)))
        {
            printf("%s\n", dir->d_name);
        }
        closedir(dp);
    }
    return 0; //if we get here, we were successful and should thus return 0
}

/**
 * @brief my built in function for getting the environment variables
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int return success state
 */
int exec_environ(char **args)
{
    //if a second argument is provided, we should output an error
    if (args[1] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }

    //Error checking print statements (commented out as they are not necessary right now):
    // char* my_env_var = getenv("MY_ENV_VAR");

    // if(my_env_var)
    //     printf("Var found: %s", my_env_var );
    // else
    //     printf("Var not found.");   
    //int j = 0;
    // int numVars = sizeof(myEnvironArr)/sizeof(myEnvironArr[0]);
    //printf("Num %d", numVars);
    //char* vars[] = {"USER", "HOME", "PATH", "SHELL", "OSTYPE", "PWD", "GROUP"};

    int i= 0;
    while (strcmp("_", myEnvironArr[i]) != 0)
    {
        //For error checking:
        //printf("My Environ Arr: %s", myEnvironArr[i]);

        const char* v = getenv(myEnvironArr[i]);
        if(v != NULL) 
        {
            printf("%s=%s\n", myEnvironArr[i], v);
        }
        else 
        {
            //printf("Error: Could not find environment variable %s.\n", myEnvironArr[i]);
        }
        i++;
    }
    return 0; //if we get here, we were successful and should thus return 0
}

/**
 * @brief my built in function for the echo command
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int return success state
 */
int exec_echo(char **args)
{
    //creating a string to hold the echo-d string
    char *echo_string = malloc(sizeof(char *) * MAX_LENGTH);
    int i = 1; //counter variable
    while (args[i] != NULL)
    {
        strcat(echo_string, args[i]);
        strcat(echo_string, " "); //we do it this way to remove the extra spaces added in
        i++;
    }
    printf("%s\n", echo_string);
    strcpy(echo_string, ""); //resetting the value of echo_string for next time
    free(echo_string); //freeing the value
    return 0; //return 0 to indicate success
}

/**
 * @brief build in help function that opens up the readme
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int returns success state
 */
int exec_help(char **args)
{
    //if a second argument is provided, we should output an error message
    if (args[1] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }
    char *help = malloc(sizeof(char *) * MAX_LENGTH);
    strcat(help, "more ");
    strcat(help, originalDir);
    strcat(help, "/readme_doc.md");
    system(help);
    //execlp("more", "more", "README.md", NULL);
    strcpy(help, "");
    free(help);
    return 0; //return 0 to indicate success
}

/**
 * @brief my built in function to pause the shell
 * 
 * @param args arguments inputted into the terminal by the user
 * @return int returns success state
 */
int exec_pause(char **args)
{
    //if a second argument is provided, we should output an error message
    if (args[1] != NULL)
    {
        fprintf(stderr, "%s\n", "Too many argument.");
        return -1;
    }
    //Otherwise, we should pause operation:
    char c;
    printf("Operation paused. Press enter to continue.\n");
    while ((c = getchar()) != '\n') //having nothing in this while loop pauses the operation essentially; once the user clicks enter (types \n) we exit the while loop and continue.
    {
    }
    return 0; //returning 0 to indicate success
}

//END OF MY PROGRAM :)