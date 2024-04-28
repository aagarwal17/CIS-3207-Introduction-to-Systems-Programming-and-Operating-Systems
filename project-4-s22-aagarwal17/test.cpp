/**
 * @file test.cpp
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief This is simply a program that contains all the provided code!
 * @version 0.1
 * @date 2022-04-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>

int main()
{
    // Once the signal handler has been created, you still need to make sure that the signal is actually handled
    // It’s probably not a good idea to overwrite SIGINT in practice, ^C no longer kills your program
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
