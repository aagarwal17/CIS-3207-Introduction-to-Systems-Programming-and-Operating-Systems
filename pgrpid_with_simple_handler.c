/* 
  The program installs a handler for SIGINT and then forks. Signal
  dispositions are inherited by the child (i.e., upon receipt of a
  signal both parent and child will act on it in the same way).

  The handler function does nothing but set a global flag, which when
  set to 1, is an indication to quit.



  To test the program, we open two terminals on the same machine. In
  one terminal, we run the program, noting the process IDs of parent
  and child, and their process group ID. We see that the parent and
  child have the same process group ID, which is the same as the
  process ID of the parent.

  In the 2nd terminal, we have the ability to send a signal to an
  individual process or to all processes in the process group using
  the kill command.

  The syntax to send a signal to an individual process:

     kill -SIGNAL_NAME PID

  and to send a signal to all processes in a given process group:

     kill -SIGNAL_NAME -PGID

  (notice the '-' before the process group id.)
  
  In a sample run of the program, the output is:
  
     I'm the parent with pid 25040 and pgrpid 25040
     I'm the child with pid 25041 and pgrpid 25040

  In another terminal, we can send a SIGINT to the parent by running
  the command:

     kill -SIGINT 25040

  The parent quits, but the child is still running. We can send a
  SIGINT to the child by running the command:
  
     kill -SIGINT 25041

  We could instead have sent a SIGINT to both parent and child by
  sending the signal to the entire process group. This would be:

     kill -SIGINT -25040

*/
  
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

volatile sig_atomic_t quittin_time;

void SIGINT_handler(int);

int main(void) {
  pid_t pid;

  quittin_time = 0;

  /* establish handler for both parent and child */
  if (signal(SIGINT, SIGINT_handler) == SIG_ERR) {
    fprintf(stderr, "error installing SIGINT handler\n");
    return EXIT_FAILURE;
  }

  if ((pid = fork()) == -1) {
    fprintf(stderr, "fork error\n");
    return EXIT_FAILURE;
  } else if (pid == 0) {
    printf("I'm the child with pid %d and pgrpid %d\n",
           getpid(), getpgrp());

    while (!quittin_time) {
      pause();  /* do nothing until we receive a signal */
    }

    printf("I'm the child. Time to quit.\n");
  } else {
    printf("I'm the parent with pid %d and pgrpid %d\n",
           getpid(), getpgrp());

    while (!quittin_time) {
      pause(); /* do nothing until we receive a signal */
    }

    printf("I'm the parent. Time to quit.\n");
  }
  
  return EXIT_SUCCESS;
}

void SIGINT_handler(int sig) {
  quittin_time = 1;
}
