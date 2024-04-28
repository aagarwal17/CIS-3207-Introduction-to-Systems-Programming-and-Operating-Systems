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
}

/**
 * @brief signal handler function to handle SIGINT signal
 *
 * @param signal
 */
void signal_handler(int signal)
{
    if (signal == SIGUSR1)
    {
        puts("child received SIGUSR1");
    }
    else if (signal == SIGUSR2)
    {
        puts("child received SIGUSR2");
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
