# Project-2-SP22
Development of a Linux Shell
Arun Agarwal
# # Project 2: Developing a Linux Shell

----------MYSHELL MANUAL----------
 To access this manual from the shell: "help"
 Press enter to read the next line of the manual, or press q then enter to quit and return to the shell.
----------------------------------
 Contents: 
   1. Introduction/Description
   2. Builtin Commands
   3. Background Execution
   4. Input/Output Redirection
   5. Piping
   6. Bash Commands
   7. About Me/Conclusion
----------------------------------
 1. Introduction/Description:

    For this project, I have built a simple Unix/Linux shell in C. he shell is the heart of the command-line interface, and thus is central to the Linux/Unix/C programming environment. Mastering use of the shell is necessary to become proficient in this world; knowing how the shell itself is built is the focus of this project. I have implemented a **command line interpreter (CLI)** or, as it is more commonly known, a **shell**. The shell should operate in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then prompts for more user input when it has finished. 

    Through this assignment, I complete three main objectives:
    - To further familiarize yourself with the Linux programming environment.
    - To learn how processes are created, destroyed, and managed.
    - To gain exposure to the necessary functionality in shells.

    In order to Execute:
    The user can open myshell by either typing ./myshell, which opens the program's interactive mode, or ./myshell batchfile (.txt file), which opens the batch mode.

    The shell has two modes of functionality:
    - Interactive Mode (read/execute loop): myshell will continuously take in user commands until the user type in the exit command. That is, the codes in the main function can be executed with a while loop, and the system will repeated until the user call the quit command.
    - Batch Mode (read commands from a file): While in batch mode, myshell will read and execute the commands from a text file until the end of the file is reached. 

    The code for internal commands, program I/O, and file execute/read/write will be implemented using the appropriate system calls.

    To process user commands, the functions from string.h library are used, such as strtok() (used to tokenized the string into separate words). An implementation of sets (data structure) is used to check if the command contains members of a set of internal commands. All other commands are treated as program invocation and will be handled accordingly (using fork() and exec(). Proper error messages are displayed using the errno.h library and the stderr file descriptor.
----------------------------------
 2. Builtin Commands
    These are the internal commands my shell supports:
    cd [directory]
       Changes current working directory to the one with the relative path specified in the [directory] argument. If the argument is not present, report the current directory. If the directory does not exist, display the error message to the user. Also change the PWD environment variable. 
    clr
       Clears the terminal screen (note: one can also scroll up to see the old commands inputted into the terminal due to the way clr is implemented )
    dir [directory]
       Prints a list of the contents of the specified directory. If no directory is specified, prints contents of current working directory.
    environ
       Prints current environment PATH strings. These are obtained from getenv() function
    echo [comment]
       Prints [comment] inputted as second argument to the screen.
    help
       Prints the contents of this manual. More of this manual can be seen in the terminal by clicking the down arrow or pressing enter (essentially using the more filter).
    pause
        Stops the shell operation until the 'Enter' key is pressed
    quit 
       Exits/quits this shell.
----------------------------------
 3. Background Execution
    In the normal paradigm for executing a command, the parent process creates a child process, starts it executing the command, and then waits until the child process terminates. If the "and" ("&") operator is used to terminate the command line, then the shell is expected to create the child process and start it executing on the designated command but not have the parent wait for the child to terminate. That is, the parent and the child will execute concurrently. While the child executes the command, the parent prints another prompt to stdout and waits for the user to enter another command line. If the user starts several commands, each terminated by an "&", and each takes a relatively long time to execute, then many processes can be running at the same time. 
 
    When a child process is created and starts executing as its own program, both the child and the parent expect their stdin stream to come from the user via the keyboard and for their stdout stream to be written to the character terminal display. Notice that if multiple child processes are running concurrently and all expect the keyboard to define their stdin stream, then the user will not know which child process will receive data on its stdin if data is typed to the keyboard. (do they all receive the same data?) Similarly, if any of the concurrent processes write characters to stdout, those characters will be written to the terminal display wherever the cursor happens to be positioned. The kernel makes no provision for giving each child process its own keyboard or terminal (unlike a windows system, which controls the multiplexing and demultiplexing through explicit user actions). 

    Now, for my shell, if the user types in the ampersand sign (&) at the end of the command line , myshell will immediate return to taking in commands, while the command in the background is still being processed.
----------------------------------
 4. Input/Output Redirection:
    A process, when created, has three default file identifiers: stdin, stdout, and stderr. These three file identifiers correspond to the C++ objects cin, cout, and cerr. If the process reads from stdin (using cin) then the data that it receives will be directed from the keyboard to the stdin file descriptor. Similarly, data received from stdout (using cout) and stderr (using cerr) are mapped to the terminal display. The user can redefine stdin or stdout whenever a command is entered. If the user provides a filename argument to the command and precedes the filename with a “less than” character "<” then the shell will substitute the designated file for stdin; this is called redirecting the input from the designated file. 

    Now, my shell accepts such i/o redirection commands.
    > [outputfile]   redirects output from the screen to [outputfile], replacing the existing file if it exists. Usable on internal commands dir, environ, echo, and help, and any executable files.
    >> [outputfile]  redirects output from the screen to [outputfile], appending to the existing file if it exists. Usable on internal commands dir, environ, echo, and help, and any executable files.
    < [inputfile]    redirects input from the keyboard to [inputfile]. Usable on any executable files.

    As an example,  the command line
        programname arg1 arg2 < inputfile > outputfile
    will execute the program /programname/ with arguments /arg1/ and /arg2/, the stdin FILE stream replaced by /inputfile/ and the stdout FILE stream replaced by /outputfile/. 

    To handle I/O redirection, system calls such as fork(), dup(), and dup2(), can be used along with the proper file descriptors.
----------------------------------
 5. Piping:
    The pipe is a common IPC (InterProcess Communication) mechanism in Linux and other versions of UNIX. By default, a pipe employs asynchronous send and blocking receive operations2. Optionally, the blocking receive operation may be changed to be a non-blocking receive. Pipes are FIFO (first-in/first out) buffers designed with an API that resembles as closely as possible a low level file I/O interface. A pipe may contain a system-defined maximum number of bytes at any given time, usually 4KB. A process can send data by writing it into one end of the pipe and another can receive the data by reading the other end of the pipe. 
    Thus, multiple operations can be strung together using the pipe (|) operator in my shell. For example,
	 [program1] | [program2]
    will execute program1, then pass its output as the input to program2. This is the same as 
	 [program1] > somefile
	 [program2] < somefile
    
    Multiple commands can be piped together. The first command/executable must give some output, and the second command/executable must take input as a parameter.
----------------------------------
 6. Bash Commands:
    To invoke the program, myshell will treat all commands that are not built in commands as program invocation and try to execute them using fork() and exec(). That is, we will create a child process to run these bash or external commands. Thus, bash commands can be used just the same as they would be able to be used in the bash shell.

----------------------------------
 7. Author: Arun Agarwal
    Class: CIS 3207
    Date: 03/23/2022

    Conclusion:
    We were able to successfully create a simple unix/linux shell in C with the following commands and built-in functions available only with one minor error/issue!
----------------------------------