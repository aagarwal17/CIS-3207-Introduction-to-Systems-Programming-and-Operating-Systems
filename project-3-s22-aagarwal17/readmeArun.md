# Project 3: CIS 3207, The Multi-Threaded Echo Server
## By: Arun Agarwal, April 5th, 2022

# Brief Project Introduction:
For this project, we create a server that can service multiple clients in echoing a message that they have sent out. Echo servers are often used to demonstrate and test networked communication. I have created an echo server and echo client and evaluated their performance (see end of readme). The purpose of the assignment is to gain some exposure and practical experience with multi-threaded programming and the synchronization problems that go along with it, as well as with writing programs that communicate across networks.  

Through completion of this project/assignment, I have learned a bit about network sockets in lecture and lab. If one would like to learn more detailed information on this topic, it is available in Chapter 11 of Bryant and O’Hallaron, and Chapters 57-62 in Kerrisk.  [Beej’s Guide](http://beej.us/guide/bgnet/)  and  [BinaryTides’ Socket Programming Tutorial ](http://www.binarytides.com/socket-programming-c-linux-tutorial/) are also useful online resources. These resources were made use of to complete this assignment.

The high-level view of network sockets is that they are communication channels between pairs of processes, somewhat like pipes. They differ from pipes in that a pair of processes communicating via a socket may reside on different machines, and that the channel of communication is bi-directional. Much of what is considered to be “socket programming” involves the mechanics of setting up the channel (i.e., the socket) for communication. Once this is done, we’re left with a socket descriptor, which we use in much the same manner as we’ve used descriptors to represent files or pipes.  Thus, we have developed a server program that echoes a text message on demand. The echo server is a process that will read sequences of words (sentences) sent by clients. The sentences (text strings), sent by a client computer, will be sent back to the requesting client.

# Program Design Description:
I followed a very distinct design methodology to be able to complete this project successfully. I will go through all the important pieces of my project and how they work together.

First, I separate my program into a few files:
1. makefile: compiles all my files, allowing multi-threaded server to run
2. echoServer.c: my main code file
3. echoServer.h: header file for my main code file that contains icnlude statements as well as structs
4. log.txt: text file to contain log file information (discussed later)
5. client.c: simple client program
6. providedEchoProgram.c: the intial code provided by the Professor for this assignment
7. readme.md: The instructions and description provided for this assignment

Looking a bit more into echoServer.h, it can be noted that it includes many c libraries, a struct for the nodes/cells (which has the client, the word/message, the client socket, and a pointer to the next node), a struct for the queue used in the connection buffer/job queue and the log queue (which has the front node of the queue and the queue size), and the function declarations for my functions in echoServer.c.

Next, looking at the beginning of my echoServer.c file, we see that it contains variables for the default values (default port set to 8088 and default message length set to 256), global variables to hold the true/false values for the flag parser, variables to hold the various parameters, locks and condition variables to be used throughtout the program, and a declaration for the job queue and log queue (because there should only be one of each for the instance of the program). For my program, I end up needeing a mutex lock for the log file, the log queue, and the job queue. I also end up needing four condition variables.

Moving on, I will discuss my main server thread/main function. My program takes as a command line several control parameters. These parameters are identified using flags:
-m is used if the user wants to set a length for their messages. If not, a default length of 256 is used.
-p is used if the user wants to set a port to be used. If not, a default port of 8088 is used.
-c is required and sets the number of cells/nodes for the program.
-w is required and sets the number of worker threads for the program.
-t is required and sets the character to represent the termination of the program.

Thus, my program can have a total of 5 parameters, and as little as 3 parameters (no port or message length/buf size provided). Numerous if/else statements are included to error check the parameters inputted by the user. 

My main server thread (the main fucntion) is performing two primary functions: 
1) Accepting and distributing connection requests from the clients
2) Constructing a log file of all the echo activities

When the server starts, my main thread creates a fixed-sized data structure which will be used to store the socket descriptor information of the clients that will connect to it. The number of elements in this data structure (shared buffer) is specified by a program input parameter (‘size of the connection buffer’). The main thread creates a pool of worker threads (‘the number of threads’ specified as a program parameter), and then the main thread immediately begins to behave in the following manner (to accept and distribute connection requests):  

while (true) 
{  
   connected_socket = accept(listening_socket);  
   add connected_socket information to the work buffer;  
   signal any sleeping workers that there’s a new socket in the buffer;  
}  

A second server thread will monitor a log queue and process entries by removing and writing them to a log file.
 
while (true) 
{  
   while (the log queue is NOT empty)
    {  
       remove an entry from the log  
       write the entry to the log file  
    }  
 }

In general, I instantiate the job and log queues, instantiate the locks and conditions variables, create the desired number of worker threads, create the log thread, set up the socket, run the while loops as pointed out above (with error checking throughout), and return 0 at the end to indicate success.

Next, I have taken a function from the textbook called open_listenfd(int port), which is used in the mains server thread to create the socket descriptor, and binds the socket descriptor for the specified port. This is equivalent to the get_socketdescriptor() function that the Professor provides. The code of this function was not necessary to be understood for this assignment, but if one would like to find out the details, they can reference the textbook.

Next, I have a function to create a queue, with error checking included. I make sure to set the front of the queue to null, the size to 0, and to malloc space. 

Then, I have a function for deleting the queue for when we are done with it. Here, I basically just call free.

Moving on, I have a function named create_node to create the node/cell object. I allocate space for the node, error check, allocate space for the message/word, error check, set certain values of the node to null/0, and return the created node.

Then, I have a function for pushing the created node onto the queue. The function creates a new node, and has two main cases: an empty queue or non-empty queue. If the queue is empty, the node is added to the front of the queue (and the front variable is set accordingly). If not, we set the node in the queue to the value of next until we find an empty space (while loop iteration). We also make sure to increase the queue size by 1. 

After this, I have a function named pop for popping off the first node struct from the queue, which is to be done after we are ready to use that node. We want to "pop" it off the queue to indicate we no longer have to service it. If the queue is empty, we simply return null, but otherwise, we make sure to reorganize the values of next and front accordingly. We of course return the popped off node/cell.

From here comes one of main functions, the worker_thread function. This is, as the name suggests, my main function for all worker thread related jobs. I first write out some message strings to print out to the user/terminal for different cases, inlcuding a prompt message, a close/termination message, and an error message. The cells in the connection buffer are filled by the main server thread. The cells are processed by the worker threads. Each cell is to contain the connection socket, and the time at which the connection socket was received. We run a while loop, as described in the instructions:
while (true) 
{  
   while (the work queue is NOT empty) 
    {  
      remove a socket data element from the connection buffer  
      notify that there’s an empty spot in the connection buffer  
      service the client  
      close socket  
   }  
}

More specifically, we use recv() and send() to talk with the client, send a prompt to the client, get the time the request was sent (using ctime_r()), check if there was any errors in the process (and send an error prompt accordingly), quit the program if the user enters the termination character (and print out "ECHO SERVICE COMPLETE> Time of request from client is []. Termination time is []", with the times filled in accordingly), and echo the message/word of the user and send it the log file with the time of completion. For both the termination case and the echoing case, I lock the log queue because no more than one worker thread should manipulate it at a time. I check to see if the queue is full (and wait if needed), I push the message to the log file, unlock the log queue, and send the signal. 

My next main function is the log_thread function. As outlined in the instructions, a second server thread monitors a log queue and proceses entries by removing and writing them to a log file in the following manner:
while (true) 
    {
        while (the log queue is NOT empty) 
        {
            remove an entry from the log
            write the entry to the log file
        }
    }
Thus, while true, I lock the log queue, wait if empty, get the message from the top/front node/cell, release the lock on the log queue, send the signal, make sure to error check if the user inputs no message, lock the log file, write the results to the log file, and then unlock the log file. 

Finally, I include the provided readLine() function from the textbook for reading inputted lines. 


In terms of the design of the client code, it follows the following format:
while (there's a message to read) 
{
    read message from the socket
    if (the message is NOT the message terminator) 
    {
        echo the reversed message back on the socket to the client;
    } 
    else 
    {
        echo back on the socket "ECHO SERVICE COMPLETE";
    }
    write the received message, the socket response message or “ECHO SERVICE COMPLETE”) and other log information to the log queue;
}
Most of the code for the client program was provided by the professor, only changing the information for the message length and port number.

# Testing, Results, and Debugging Description:
To begin, to test that the program works, many different methods were tested.

I initially tested my program by just running the provided code to make sure I understood the fundamentals of how multi-threaded servicing works.

I then built my code to work for multiple clients at once, making sure to use print statements and if/else loops for error checking throughout the process of writing my code, and to permanently keep in the code. 

These error cases are described in the description above. In general, I check for improper parameters, errors with connecting, and errorrs with sizes/empty objects. 

After this code was written, I ran the server and the client program on my own computer. When doing this, my server's network address is the loopback address of 127.0.0.1. This could also be accessed by just typing localhost. 

More specifically, in one terminal/bash pane, I would compile and write ./echoServer -m 256 -p 8088 -c 1000 -w 10 -t ; and in multiple other terminals, I would write telnet 127.0.0.1 8088. This would allow me to test multiple cases of my program. For example, I could test what happens when we did and did not include certain parameters, made sure that the if/else error checking was working as intended, and made sure my program could handle various cases. For example, I tried various terminating characters, various numbers for the buffer size and number of worker threads (to ensure proper synchronization and performance under varying loads of quests), different ports, and more. As an example, it was reccomended that I try setting 20 worker threads with a small buffer and a large buffer, and both ended up working fine. 

In general, I used the gbd debugger throughout the creation of my program for various segmentation faults and other errors. I had to write a launch.json file to use the debugger.

As another test, I used my client program to see if that would also work. I also used the cis-linux2 systems to run multiple simultaenous instance of my client on other systems, which worked with no issues (on the localhost of course). 

Doing all of these things allowed me to effectively test that my program worked.
NOTE: I am not including screenshots of my testing in this readme document because I am unsure of how to include pictures in such a document.
However, I would be more than happy to show that all the things I tested work as I describe during demo.

In terms of results, I was able to succesfully create the multi-threaded echo server, working locally or on a real network (cis-linux2 systems). My program would correctly service the client and output the desired messages to the terminal and/or log file, as specified in the instructions. I was, however, running into one main issue. I wasn't sure how to write the client program, and the instructions did a poor job in explaining/helping us to write one. Thus, after speaking with the Professor, he said it would be alright to just use the client program that he provided, changing a few variable names to work for my program. Thus, I did exactly this. Unfortunately, due to my lack of understanding of how the client program works, there is some error occuring with it that is causing it to continuously prompt the user. Nonetheless, my program works correctly for the major use cases, so I overall succeeded in completing the goals of this project (making the echo server that would service multiple clients).

In terms of a debugging description, I describe this in the testing section above. I debugged my program by using print statements to pinpoint exactly where the errors were occuring. I also used the gdb debugger for hard to deal with errors such as segmentation faults. By going through the logic and doing research into all the functions/methodologies used for this assignment, I could accurately debug and complete the program.

# Discussion and Analysis:
Overall, I was able to create the main server thread, create the worker threads and log manager thread, create the buffer and management for socket descriptor elements from the main server thread, create management of socket descriptors by the worker threads, create the main thread socket for listening for incoming requests from clients, create the client send request socket for communication with main server thread, create worker thread insertion in the log buffer, create worker thread communication with the client for echoing, create log manager thread updates to log file, create client generation and message sending, and test with single and multiple clients and varied intervals of requests, buffer sizes and numbers of worker threads for message echo.

To conclude, I was able to succesfully create the multi-threaded echo server, working locally or on a real network (cis-linux2 systems). My program would correctly service the client and output the desired messages to the terminal and/or log file, as specified in the instructions. I was, however, running into one main issue. I wasn't sure how to write the client program, and the instructions did a poor job in explaining/helping us to write one. Thus, after speaking with the Professor, he said it would be alright to just use the client program that he provided, changing a few variable names to work for my program. Thus, I did exactly this. Unfortunately, due to my lack of understanding of how the client program works, there is some error occuring with it that is causing it to continuously prompt the user. Nonetheless, my program works correctly for the major use cases, so I overall succeeded in completing the goals of this project (making the echo server that would service multiple clients).

I made sure to make my code as readable, correct, organized, efficient, and tested as I could. I was able to better understand locks and condition variables, ports, ip addresses, multithreaded servicing, and many other C and programming concepts! This was a great assignment, and I am glad I was able to succeed in completing it!