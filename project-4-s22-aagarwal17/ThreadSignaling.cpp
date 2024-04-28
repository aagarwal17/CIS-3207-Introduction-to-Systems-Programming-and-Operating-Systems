/**
 * @file ThreadSignaling.cpp
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief My main program for singaling with multiple threads (multi-threading)
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//including my header file obviously
#include "ThreadHeader.h"

/**
 * @brief Defining some standard values to use throughout the program
 * 
 */
#define TIME_TO_RUN -1
#define MAX_SIGNAL_COUNT 100000 //I am setting a limit for testing/possible issues
#define SUB_PROCESSES 8 //this is how I saw this being set up online. I realize the instructions do not say to do this, but I was having trouble figuring out this assignment, and I found a tutorial that split it up like this.

//Global Variables
Counters *count; //counter being used for signals
int concurrentProcesses;
pthread_t processType[8];

struct timespec timeSIGUSR1;
double timeSummedSIGUSER1;

struct timespec timeSIGUSR2;
double timeSummedSIGUSER2;

/**
 * @brief main function allowing program to run
 * 
 * @return int 
 */
int main()
{

    srand(time(NULL));

    count = (Counters *)malloc(sizeof(Counters));

    //As we have specified (in lab and in instructions), we should start by masking/blocking SIGUSER1 and SIGUSER2 in the main function:
    block_signal(SIGUSR1);
    block_signal(SIGUSR2);

    //Initializing all mutexes for both
    pthread_mutexattr_t attributeOne;
    pthread_mutexattr_init(&attributeOne);
    pthread_mutexattr_setpshared(&attributeOne, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(count->mutexOne), &attributeOne);

    pthread_mutexattr_t attributeTwo;
    pthread_mutexattr_init(&attributeTwo);
    pthread_mutexattr_setpshared(&attributeTwo, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(count->mutexTwo), &attributeTwo);


    // Creating the threads. I am doing this a little bit differently than the instructions have laid out becuase I could not figure out how to do this the same way the instructions describe.
    //I instead have a counter that goes to 8 and I create threads based on that (one for SIGUSER1, one for SIGUSER2, one for reporting, and a final extra one I add for generating).
    //While this is not exactly how the instructions tell us to do this, I am still acomplishing the same goal
    //int i = 0;
    for (int i = 0; i < SUB_PROCESSES; i++)
    {
        //Message for terminal so that I know that it is functioning correctly
        cout << "I = " << i << endl;

        if (i < 2) //Creating thread for SIGUSER1
        {
            pthread_create(&processType[i], NULL, &signalUpdater, (void *)SIGUSR1);
        }
        else if (i >= 2 && i < 4) //Creating thread for SIGUSER2
        {
            pthread_create(&processType[i], NULL, &signalUpdater, (void *)SIGUSR2);
        }
        else if (i == 4) //Creating thread for reporter
        {
            pthread_create(&processType[i], NULL, &reporter, (void *)NULL);
        }
        else if (i > 4 && i < 8) //Creating thread for generator
        {
            pthread_create(&processType[i], NULL, &generator, (void *)NULL);
        }
    }

    if (TIME_TO_RUN == -1)
    {
        while (true)
        {
            if (maxCountReached())
            {

                for (int y = 0; y < SUB_PROCESSES; y++)
                {
                    // If we reach the max count that we set, kill all child processes.
                    cout << "Attempting to kill process " << y << " " << processType[y] << endl;
                    pthread_kill(processType[y], SIGINT);
                }

                sleep(2);

                cout << count->sentSIGUSR1 << " " << count->receivedSIGUSR1 << endl;
                cout << count->sentSIGUSR2 << " " << count->receivedSIGUSR2 << endl;

                exit(0); //exiting when successful
            }
        }
    }
    else
    {
        cout << "MADE IT TO THIS POINT" << endl;
        sleep(TIME_TO_RUN);
        cout << "MAIN FUNCTION PID " << getpid() << endl;

        cout << sizeof(processType) / sizeof(processType[0]) << endl;

        sleep(2);
        cout << count->sentSIGUSR1 << " " << count->receivedSIGUSR1 << endl;
        cout << count->sentSIGUSR2 << " " << count->receivedSIGUSR2 << endl;

        exit(0); //exiting when successful
    }

    return 0;
}

/**
 * @brief my function for the generator:
 * 
 * @param args 
 * @return void* 
 */
void *generator(void *args)
{
    //Indicating in terminal that generator is starting
    cout << "Generator Starting" << endl;

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);


    while (true)
    {

        struct timespec time_delay;
        time_delay.tv_sec = 0;
        time_delay.tv_nsec = randGenerator(0.01, 0.1) * 1000000000.0;
        nanosleep(&time_delay, NULL);

        //We need to randomly send signals for either SIGUSER1 or SIGUSER2
        //To do this, I am creating a random number and using a random number generator that goes between 0 and 1
        //If it is less than .5, we send SIGUSER1
        //If it is equal to or above .5, we send SIGUSER2
        //There may be other options for random generation, but this method seems to work well
        double theRandomNum = randGenerator(0.0, 1.0);
        if (theRandomNum < 0.5)
        {
            cout << "Sent SIGUSR1 " << endl;
            //sending kill to the 3 threads:
            pthread_kill(processType[0], SIGUSR1);
            pthread_kill(processType[1], SIGUSR1);
            pthread_kill(processType[4], SIGUSR1);

            pthread_mutex_lock(&count->mutexOne);  //creating lock and locking because of shared access 
            count->sentSIGUSR1++; //incrementing
            pthread_mutex_unlock(&count->mutexOne); //unlocking lock
        }

        //Basically same process as above but for SIGUSER2:
        else if (theRandomNum >= 0.5)
        {

            cout << "Sent SIGUSR2 " << endl;
            //sending kill to the 3 threads:
            pthread_kill(processType[2], SIGUSR2);
            pthread_kill(processType[3], SIGUSR2);
            pthread_kill(processType[4], SIGUSR2);

            pthread_mutex_lock(&count->mutexOne);
            count->sentSIGUSR2++;
            pthread_mutex_unlock(&count->mutexOne);
        }
    }
    return NULL;
}

/**
 * @brief my function for blocking/unblocking signals as well as printing messages to the terminal to indicate status of signal:
 * waiting, sending, releasing, receiving...
 * 
 * @param args 
 * @return void* 
 */
void *signalUpdater(void *args)
{
    //Printing to terminal SIgnalUpdater
    cout << "SignalUpdater " << endl;

    sigset_t sigset;
    int sig;
    sigemptyset(&sigset);

    if (args == (void *)SIGUSR1)
    {
        sigaddset(&sigset, SIGUSR1);
    }
    if (args == (void *)SIGUSR2)
    {
        sigaddset(&sigset, SIGUSR2);
    }

    //Same as provided layout, we run while true:
    while (true)
    {

        cout << "Waiting... signalUpdater" << endl;
        sigwait(&sigset, &sig);
        cout << "Releasing... signalUpdater" << endl;

        if (sig == SIGUSR1) //If SIGUSER1, print following messages and lock the lock, updated count for the signal and then unlock the lock:
        {
            cout << "Received SIGUSR1 " << endl;
            //As described in the instructions, I place a mutex to lock the shared memory and then updated receivedSIGUSER1 count
            pthread_mutex_lock(&count->mutexTwo);
            count->receivedSIGUSR1++;
            pthread_mutex_unlock(&count->mutexTwo);  //now updated, we unlock the lock
        }
        if (sig == SIGUSR2) //If SIGUSER2, print following messages and lock the lock, updated count for the signal and then unlock the lock:
        {
            cout << "Received SIGUSR2 " << endl;
            //As described in the instructions, I place a mutex to lock the shared memory and then updated receivedSIGUSER1 count
            pthread_mutex_lock(&count->mutexTwo);
            count->receivedSIGUSR2++;
            pthread_mutex_unlock(&count->mutexTwo);  //now updated, we unlock the lock
            cout << "Finished SIGUSR2 " << endl;
        }
    }

    return NULL;
}

/**
 * @brief this is the function which focuses on reporting to the log file
 * 
 * @param args 
 * @return void* 
 */
void *reporter(void *args)
{

    sigset_t sigset;
    int sig;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGINT);

    ofstream log;

    //open a log file and call it logT.txt to hold the info generated for multi-threading with signals:
    log.open("logT.txt", ios::app);
    if (!log.is_open()) //error checking
    {
        cout << "Couldnt open file correctly" << endl;
    }

    cout << "Reporter Starting " << endl; //letting terminal know that reporting is starting

    //while true, we are going to have the reporting thread in the receiving process monitor the receive signal counter
    //When an aggregate of 20 signals have arrived, I log the signal type, receive time, the receiving thread ID for those signals, and the average time between signals to a file
    while (true)
    {

        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now()); //getting current time

        cout << "Waiting... reporter" << endl;
        sigwait(&sigset, &sig);
        cout << "Releasing... reporter" << endl;

        if (sig == SIGUSR1) //if SIGUSER1, get time sum and print to termianl
        {
            cout << "\t\t\tReporting SIGUSER1 " << getppid() << " -> " << getpid() << endl;
            clock_gettime(CLOCK_REALTIME, &timeSIGUSR1);
            timeSummedSIGUSER1 += timeSIGUSR1.tv_sec + timeSIGUSR1.tv_nsec;
            timeSummedSIGUSER1 /= 1000000000L;
            cout << "timeSum " << timeSummedSIGUSER1 << endl;
        }
        else if (sig == SIGUSR2) //similarly if SIGUSER2
        {
            cout << "\t\t\tReporting SIGUSER2 " << getppid() << " -> " << getpid() << endl;
            clock_gettime(CLOCK_REALTIME, &timeSIGUSR2);
            timeSummedSIGUSER2 += timeSIGUSR2.tv_sec + timeSIGUSR2.tv_nsec;
            timeSummedSIGUSER2 /= 1000000000L;
        }

        concurrentProcesses++; //incrementing
        
        //If we have aggregated 20 signals, we log the information specified earlier:
        if (concurrentProcesses == 20)
        {
            //Creating lock because of shared access
            pthread_mutex_lock(&count->mutexTwo);

            //getting the average time for both SIGUSER1 and SIGUSER2 by dividing by 20 (number of signals)
            double avgTime1 = timeSummedSIGUSER1 / 20.0;
            double avgTime2 = timeSummedSIGUSER2 / 20.0;

            //Adding the information to the log file:
            log << "SIGUSR1 | "
                << " | Sent " << count->sentSIGUSR1 << " Received " << count->receivedSIGUSR1 << " | "
                << "Current Time " << ctime(&currentTime) << " | Avg Time Between Signals " << avgTime1 << endl;
            log << "SIGUSR2 | "
                << " | Sent " << count->sentSIGUSR2 << " Received " << count->receivedSIGUSR2 << " | "
                << "Current Time " << ctime(&currentTime) << " | Avg Time Between Signals " << avgTime2 << endl;
            log << endl;

            //Resetting the average time since we are donw with them now
            avgTime1 = 0;
            avgTime2 = 0;
            concurrentProcesses = 0;

            //We are done accessing shared state, so we unlock the locks:
            pthread_mutex_unlock(&count->mutexTwo);
        }
    }

    return NULL;
}
/**
 * @brief Very simple function for random number generation taken from GeeksForGeeks
 * 
 * @param lowerBound lower bound of time interval
 * @param upperBound upper bound of time interval
 * @return double return the randomly generated number within the bounds
 */
double randGenerator(double lowerBound, double upperBound)
{
    int ran = rand();
    double randomNum = (fmod((double)ran, upperBound * 100) + lowerBound * 100);
    return randomNum / 100;
}

/**
 * @brief I basicaly follow the same format as the provided block_signal function:
 * a sigset_t struct is  used to create the set of signals for modification
 * 
 * @param signal 
 */
void block_signal(int signal)
{
    sigset_t sigset;
    sigemptyset(&sigset); //initializing the set to empty
    sigaddset(&sigset, signal); //adding SIGINT to the set 
    int sigReturn = pthread_sigmask(SIG_BLOCK, &sigset, NULL); //modifying the mask
    if (sigReturn != 0)
    {
        cout << errno << endl;
    }
}

/**
 * @brief my function for unblocking the signal
 * 
 * @param signal the signal to unblock
 */
void unblock_signal(int signal)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signal);
    int sigReturn = pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
    if (sigReturn != 0)
    {
        cout << errno << endl;
    }
}
/**
 * @brief A function to check if we have reached the max signal count allowed
 * 
 * @return true if we have reached the max
 * @return false if we have not reached the max
 */
bool maxCountReached()
{
    int total = (count->sentSIGUSR1 + count->sentSIGUSR2);
    if (total >= MAX_SIGNAL_COUNT)
    {
        return true;
    }

    return false;
}
