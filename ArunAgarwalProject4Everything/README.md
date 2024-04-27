# Project-4-s22  Signaling with Multi-Process Programs
 
## Problem:  
This project requires _a program development_ and an analysis of performance for the solution.  

## Summary:  
The purpose of this project is to have a process and its threads responsible for servicing “signals” sent by other peer processes. The processes and threads must execute concurrently, and performance of all process and thread activities is to be monitored and analyzed.

## Learning Objectives:  
This project expands on our knowledge and experience of creating applications with multiple processes and multiple threads. The project also introduces signals as communication mechanisms. Synchronization in the form of protected access to shared variables is used.  

## Project Description:
This project is to be implemented using Linux and the signaling system calls available in Linux. A signal is an event that requires attention. A signal can be thought of as a software version of a hardware interrupt. There are many different signal types and each signal type has a defined action. The action of many of the signal types can be changed by developing a “signal handler” in code to process the receipt of a particular signal type.  

There are signals that are considered “synchronous” and there are signals considered “asynchronous”. A synchronous signal is one that is the result of an action of a particular thread such as a divide by zero error or an addressing error. An asynchronous signal is one that is not related to a particular thread’s activity and can arrive at any time, that is with no specific timing or sequencing. In this project we are dealing with asynchronous signals.  

In Linux, asynchronous signals sent to a multi-threaded process are **delivered to the process**. The decision as to which thread should handle the arriving signal is based upon the signal mask configuration of the individual threads within the process. If more than one thread has not ‘blocked’ the received signal, there is no guarantee as to which of the non-blocking threads will actually receive the signal.  

You will need to do some research about signals and signal masks. There are a number of system calls related to signals and signal masks that you can use in the implementation of your solution to the project.  

The **Program** (you are developing a single program with multiple processes) for this project requires developing a solution using a parent process, and a _signal handling_ process that contains 3 threads. In the signal handling process, two of the threads are signal receiving threads and one thread is for reporting signal receipts.  

For the **Program**, the parent process creates one child process, sets up execution of the child process and waits for its completion. In addition, the parent process behaves as a signal generator. The **Program** is to use two signals for communication: SIGUSR1 and SIGUSR2. The parent process will execute in a loop, and during each loop repetition will randomly select either SIGUSR1 or SIGUSR2 to send to the signal receiving process. Each repetition through the signal generating loop will have a random time delay ranging between .01 second and .1 second before moving to the next repetition. After generating a signal, the process will log a ‘signal sent time’ and ‘signal type’ (SIGUSR! Or SIGUSR2). The time value must have sufficient resolution to distinguish among signals sent by the processes.  

Signals should be sent with the function (look up the function and the meaning of the parameters)  
 *int kill (pid_t pid, int signum)*  

The logs of the signal sending process are to be written to a file.  

Each of the signal handling threads (in the child process) will process only one type of signal, _either_ SIGUSR1 or SIGUSR2 and ignore servicing the other signal type. (Thus, one of the signal receiving threads will handle SIGUSR1 signals and one signal receiving thread will handle SIGUSR2 signals). Each of the signal handling threads will also execute in a loop, waiting for its signal to arrive. When the signal arrives, the thread will save the signal type, time of receipt of the signal and the ID of the receiving thread in a shared buffer and increment the total count of the number of signals received. This counter and buffer are shared by the threads in the receiving process. The thread will then loop waiting for the next signal arrival.  

The reporting thread in the receiving process will monitor the receive signal counter and when an aggregate of 20 signals have arrived, it will log the signal type, receive time and receiving thread ID for those signals to a file.  

Access to shared structures by each thread requires the use of a critical section control. You are to use a lock for the critical section control. Use the lock to provide each thread exclusive access to each shared structure.  

## Results to Report:
We are interested in the performance of the threads receiving signals. This means you will need to “stress” the receiving process and its threads to see whether it is able to handle the signal service demand made by the signal senders. You may want to place additional limits (further **reducing** the loop delays) on the interval between signals sent to cause failures in signal processing.  

You will run the **Program** until a *“kill”* signal is received from a user terminal. This will let you control the length of time the program executes. You should run the program for a variety of short and long times to see if time affects signal loss.  

The kill signal sent from the terminal should cause both the sending and receiving processes to complete their operations, close their files and terminate.  

To help you control the termination of the **Program**, we have provided a sample program showing the setting up of a signal handler to ‘catch’ a signal sent to multiple processes from a user terminal. (pgrpid_with_simple_handler.c)  

There is some minimal analysis that you should perform to determine if the receiving threads are receiving all sent signals. This can easily be done using a spreadsheet (Excel, Google Sheets, …). You should read the data files into the spreadsheet to do analysis.  

You are to examine the performance; that is, the number of signals of each type sent and received. Also examine the average time between signal receptions of each type by the reporting threads. Be sure to investigate and discuss any signal losses.  

Since you are logging the times of signals sent by the sending process and the receive times by the threads, you should be able to determine when (time and conditions) the sending demand (intervals between sends) results in signal losses. This is the reason for many runs of different lengths of time as well as the variation in loop delays between signal sends.  

Be sure to include a test plan for the program, and document the results of your testing such that you can **clearly demonstrate** that the program **executes correctly**.  

You will develop your program using the supplied GitHub repository. You will submit your program code, testing documents and execution results through Canvas. Compress all of the files (do not include executables) into one file for submission. Use clear naming to help identify the project components that are submitted. Include a link to your GitHub repository as a comment to your Canvas submission.  

There will be weekly deliverables and they are to be submitted to Canvas as well.  

**Week 1**:  
* Creation of the main process, the signal catcher process and its threads. (Implement the code for each process as independent programs: one source for signal generator code, one source for signal catcher process and threads. This means use fork() and exec() in your implementation).  
* Design signal masks for the threads.  
* Design signal catcher routines  

**Week 2+**:  
* Create individual locks for shared structures.  
* Description of testing and documentation (did they include logs in processes, e.g.).   
* *Document Production*   
	* Reporter receipt and time stamping of signals and intervals for process execution to a file  
	* Description and analysis/comparison of results from execution – including multiple executions and comparison of data.  
*Final executable and Demo*  

**Grading Rubric**  
·       Creation of the main process, the additional signal receiver process, the 2 signal catcher threads and the reporter thread. (1.5 Pt)  
·       Create shared counters and individual locks for each counter (1 pt)  
·       Description of program design and comments describing each function (1 pt) 
·       Description of testing and documentation for signal senders (1 pt)  
·       Logging of signal send information at the sender processes (1 pt)  
·       Creation of proper functioning signal catcher routines (1 pt)  
·       Proper use of signal masks to enable/disable signal receptions (1 pt)  
·       Reporter receipt and time stamping of signals and intervals for Threads to a file (1 pt)  
·       Description and analysis/comparison of results from execution (1.5 pt)  
Total 10 Points  
