# Project 4: Signaling with Multi-Process Programs:
By: Arun Agarwal

## Brief Project Introduction:
For this project, we have a process and its threads, which will be responsible for servicing "signals" sent by other peer processes. The processes and threads are executing concurrently, and performance of all process and thread activities is monitored and analyzed. We implement this project using Linux and the signaling system calls provided by Linux. 

## Manual:
First, we define what is a signal. A signal is a software design of a hardware interrupt. There are many different signal types, and each signal type has a defined action. The action of many of the signal types can be changed by developing a signal handler in code to process the receipt of a particular signal type. There are signals that are considered “synchronous” and there are signals considered “asynchronous”. A synchronous signal is one that is the result of an action of a particular thread such as a divide by zero error or an addressing error. An asynchronous signal is one that is not related to a particular thread’s activity and can arrive at any time, that is with no specific timing or sequencing. In this project we are dealing with asynchronous signals.  In Linux, asynchronous signals sent to a multi-threaded process are delivered to the process. The decision as to which thread should handle the arriving signal is based upon the signal mask configuration of the individual threads within the process. If more than one thread has not ‘blocked’ the received signal, there is no guarantee as to which of the non-blocking threads will actually receive the signal. 

This lab demonstrates the use of signals for interprocess and interthread communication. As specified by the instructor, this lab used to involve programming both interprocess and interthread communication, but due to lack of time to appropiately teach all the needed material, the lab has been simplified. Because of this simplication, I was having difficulty trying to figure out how to complete a few parts of this project. I found a tutorial online that would help to guide me through writing interprocess and interthread communication, so instead of having one program for processes and threads, I decided to write two separate programs, which I believe is better in a sense as it leads to more concise results to analyze. Thus, the two types of communication are implemented in two different programs both with the goal of completing the same task. Both programs create a thread or processes respectively. Each thread has a main function, a reporting function, three signal counting functions, and three signal generating functions:
- Main - creates the threads / processes and determines when the program should stop.
- Reporter - receives signals and writes useful information / stats to a log file.
- Signal Receivers - receives and counts signals.
- Generator - creates and sends signals.

To run my program, in the terminal, input either:
-  g++ ProcSignaling.cpp -o test -pthread
-  g++ ThreadSignaling.cpp -o test -pthread
followed by ./test:
![image](https://user-images.githubusercontent.com/55926421/164813959-e5120529-1b02-4b94-8a8a-fc2049a83de5.png)

One can also run the program by using the makefile and writing make in the terminal followed by ./signal.
![image](https://user-images.githubusercontent.com/55926421/164813989-2ae553a8-8139-49c6-9069-de2a06428ed0.png)

I wrote my code in C++, so we use g++.
Once the program starts running, the information desired immediately beings being outputted to the log file. This log file is intially named either "logT.txt" when running the thread program or "logP.txt" when running the process program. These files can (and do) later have their names changed manually to reflect what the user is testing. For example, if I was doing a time comparison, in the renamed title, I would write how long I let the program run for (ex: 10sec, 30sec, 1min). If I was doing a signal count comparison, I would write the number of signals I let occur before closing the program (ex: 1000 signals). 

One of the easiest ways to end the program (which I have done for the most part throughout my testing/creation) is just do Control+C in the terminal. 

After doing this, it is up to the user to analyze the results received.

## Program Design:
In terms of desinging my program, it should be notes that I followed a distinct design methodology to be able to complete this project successfully. I will go through all the important pieces of project:

First, I separate my program into the following files:
1. makefile: compiles all my files, allowing program to run
2. ThreadSignaling.cpp: my main code file for threads
3. ThreadHeader.h: header file for thread code file that contains include statements as well as structs
4. ProcSignaling.cpp: my main code file for processes
5. ProcHeader.h: header file for process code file that contains include statements as well as structs
6. log.txt: compilation of provided code
7. prgpid_with_simple_handler.cpp: provided handler program
8. providedEchoProgram.c: the intial code provided by the Professor for this assignment
9. readme.md: The instructions and description provided for this assignment
10. testingDesign.md: This document, which contains information on testing, design, and a manual
11. resultsAnalysis.md: Other reading document that contains information on my results and their analysis
![image](https://user-images.githubusercontent.com/55926421/164814013-97c98351-37fe-41ba-a4a1-046524be90fd.png)

It should be noted that my main two code files follow almost identical code structures, other than one using threads and the other using processes (code relevant for each used), so I will describe both code files and their headers together. 

Looking at the header files, I use these for organization purposes mostly, but I also use it to create my struct. Thus, within my header files, I write all the packages I am using, the struct for Counters, and the function headers. 

Now, looking at my main code files, I start by defining global variables that will be used throughout the program. I then have my main function which sets up the shared memory, intializes the global variables being shared, does some error checking, blocks signals, sets up mutexes for both signals (SIGUSER1 and SIGUSER2), create the processes/threads (with special conditions depending on certain values), and exit indicating success. The next function is my signalHandler function, for which I follow the same design as provided by the instructions/TA. That is, I write what I would like to do for SIGUSER1 and SIGUSER2 separetely. I would like to note that, in the provided design, we edit te count and use locks within this function, but for my program, I ended up needing to write it in a separate function. Next, signalUpdater is my function for blocking/unblocking signals as well as printing messages to the terminal to indicate status of signal. Without going into the details, I would like to note that I made sure to use mutexes because of the shared access. My next function is called reportHandler, which as the name suggests, handles reporting. I include error checks and, for either SIGUSER1 or SIGUSER@, I get the time and print out the sum of time for that signal to the terminal. Moving on, my reporter function is a separate function from the reporterHandler which focuses on reporting to the log file. I unblock the signals (using a function I wrote for this described later) and signal to my reporterHandler function. I open a log file called "logP.txt" for processes. While true, we are going to have the reporting thread in the receiving process monitor the receive signal counter. When an aggregate of 20 signals have arrived, I log the signal type, receive time, the receiving thread ID for those signals, and the average time between signals to a file, as desired by the instructions. These files get a temporary name, and are manually renamed to reflect what was being tested. In this process, we have shared access, so I made sure to use mutexes. My next function is for handling the generator, generatorHandler(). The generator function is the one which handles the majority of the generator thread's duties. We need to randomly send signals for either SIGUSER1 or SIGUSER2. To do this, I am creating a random number and using a random number generator that goes between 0 and 1. If it is less than .5, we send SIGUSER1. If it is equal to or above .5, we send SIGUSER2. There may be other options for random generation, but this method seems to work well. I again use locks because of the shared access. As stated in the instructions, each repetition through the signal generating loop is going to have a randomly generating time delay ranging from .01 to .1 seconds before the next repetition, so we code this as well. The next function was taken from GeeksForGeeks and simply generates a random number between the provided boundaries, which is needed in the previous generator function. Moving on, the block_signal function follows the same format as the provided function. It is worth noting that a sigset_struct is used to create the set of signals for modification. In this function, I make sure to intialize the set to empty, add SIGINT to the set, and modify the mask. For my unblock_signal funtion, I basically follow the same process except in sigprocmask, I use SIG_UNBLOCK rather than SIG_BLOCK. Finally, I have a function for interrupting the program so that it can't run forever (called maxCountReached()). As stated earlier, the ProcSignaling.cpp and ThreadSignaling.cpp follow the same structure other than one needing threads and another using processes. Please read through the comments in all functions to get more details of my design. 

In general, I kept to following the provided design as follows:

```
int main()
{
    if (signal(SIGINT, signal_handler)) == SIG_ERR)
        {
            // error :(
        }
    while (true)
    {
        sleep(1); // don’t burn CPU cycles
    }

    block_sigint();

    while (true)
    {
        sleep(1);
    }

    pid_t pid = fork();

    if (pid == 0)
    { // child
        signal(SIGUSR1, signal_handler);
        signal(SIGUSR2, signal_handler);

        while (true)
            sleep(1);
    }
    else
    {             // parent
        sleep(1); // give child time to spawn
        parent_func();
    }
}

/**
 * @brief signal handler function to handle SIGINT signal
 *
 * @param signal
 */
void signal_handler(int signal)
{
    //Review multi-threading declarations and functions from Project 3 to spawn signal handlers
    if (signal == SIGUSR1)
    {
        puts("child received SIGUSR1");
        lock_aquire();
        signalCounter_received++;
        lock_release();
        logTime();
    }

    else if (signal == SIGUSR2)
    {
        puts("child received SIGUSR2");
        lock_aquire();
        signalCounter_received++;
        lock_release();
        logTime();
    }
    exit(0);
}
/**
 * @brief a sigset_t struct should be used to create the set of signals for modification
 *
 */
void block_sigint()
{
    sigset_t sigset;
    sigemptyset(&sigset);                  // initalize set to empty
    sigaddset(&sigset, SIGINT);            // add SIGINT to set
    sigprocmask(SIG_BLOCK, &sigset, NULL); // modify mask
}

void parent_func(int child_pid)
{
    kill(child_pid, SIGUSR1); // send USR1 signal
    puts("sent SIGUSR1 to child");
    waitpid(child_pid, NULL, 0); // wait for child
}

while (true)
{
    sleepRandomInterval(0.01, 0.1);
    signal = randomSignal();

    kill(0, signal);
    log(signal);
}
```

The readme files are self explanatory, so I will not discuss the design of them. Similarly, the log files are organized as explained earlier, with their names being descriptive to exactly what was tested for that instance. These will be discussed a bit further in the testing section next.

## Testing:
To begin, to test that I know how to write this assignment more simply, I made sure I could run the provided code, pgrpid_with_simpler_handler.cpp and understand what was going on.

Throughout writing my program,  I used the gbd debugger throughout the creation of my program for various segmentation faults and other errors. I had to write a launch.json file to use the debugger.

As another test, I had used a test program to see if that would also work. However, I ended up deleting this file because it ended up being very repetitive. 

Also, throughout writing my program, when errors popped up, I would use numerous cout statements (printing to terminal) to pinpoint exactly what the problem was. I would also keep many print statements in my code that would be used for letting the user know exactly what's going on (generator running, signal being sent, current pid, killing, etc.). 

I also commented out some code and left it in to show what I had tested to make sure my program was working as intended. 

After the program was completely written and information was being written to the log file, I had to make sure it was reliable. I intially only had the signal counts being outputted to the log files. I cannot showcase pictures of how these looked now because I did not take screenshots at the time. However, I did see that it was reliably/accurately outputting signal counts for received and sent for SIGUSER1 and SIGUSER2 by comparing the print statements I set up for the terminal to the statements being sent to the log file. Also, the numbers generally made sense because we were expecting a write at every 20 (that is what the instructions specified), and that is what I ended up seeing, of course with their being signal loss, as expected. 

In terms of actual time interval testing (as stated by the TA, we should run the program for a given time period and record results), I first set the program to run for 5 seconds and also capped the total number of signals generated to a fixed number. Once everything worked with those conditions, I allowed the program to run for 10 seconds and increased the generator cap. I kept the time at 10 seconds and removed the generator cap once everything was working. Lastly I went on to larger time intervals like 30 seconds and 1 minute and faced no issues.

Once I knew my program worked with a time limit, I removed the time cap (which I managed manually with a timer), I simply stopped my program once a certain number of signals were received manually. I am not sure if our code was supposed to do this automatically, and it didnt seem to be specified, so I just did it by hand. The results of which are analyzed in my other readme document. 

As described earlier, all log files made were named differently to reflect what was being tested in that instance:
10 Second Time Run:
- logSendReceiveProc10Sec.txt
- logSendReceiveThread10Sec.txt
30 Second Time Run:
- logSendReceiveProc30Sec.txt
- logSendReceiveThread30Sec.txt
1 Minute Time Run:
- logSendReceiveProc1Min.txt
- logSendReceiveThread1Min.txt
1000 Signals Run:
- logSendReceiveProc1000Sig.txt
- logSendReceiveThread1000Sig.txt

The results of all these files are discussed in the other read me document for analysis.

To show that my program works as expected, here are some screenshots of my results:
For Processes:
For 10 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814060-349c45a4-b2f4-4cb1-87e9-0703fd51ecfc.png)
For 30 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814083-d2b890d6-31d6-4d86-8b3b-35e20c55d8a6.png)
For 1 Minute:
![image](https://user-images.githubusercontent.com/55926421/164814103-9510293e-ea54-4356-9979-e67bb4669583.png)
For 1000 Signals Received:
![image](https://user-images.githubusercontent.com/55926421/164814187-2b7fa400-bf29-4c87-9ef3-23800c2884e1.png)
For Threads:
For 10 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814263-34587ad3-6b82-4e10-97e1-83e9b940ac40.png)
For 30 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814275-3e8bcaaa-238f-4e7b-8eff-0eae7d917cd6.png)
For 1 Minute:
![image](https://user-images.githubusercontent.com/55926421/164814302-e776db90-ad0a-424c-9b36-9fca2c467dd6.png)
For 1000 Signals Received:
![image](https://user-images.githubusercontent.com/55926421/164814326-8e148bb3-1457-4a25-ba0f-ec8d1e11ed8b.png)

All the log files include the information as explained previously, and the results are as expected to my understanding. The exact meaning of these results are discussed in the second readme file I made, resultsAnalysis.md.

Thanks!
