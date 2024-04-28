/**
 * @file myProcesses.cpp
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief Main program for signaling with multiple processes (multi-processing)
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
//including my header file obviously
#include "ProcHeader.h"

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
vector<time_t> pastSIGUSR1; //time vector for SIGUSER1
vector<time_t> pastSIGUSR2; //time vector for SIGUSER1

struct timespec timeSIGUSR1;
double timeSummedSIGUSER1;

struct timespec timeSIGUSR2;
double timeSummedSIGUSER2;

/**
 * @brief The Main function that allows the program the function
 * 
 * @return int 
 */
int main()
{

    //Setting up the shared memory and intializing the global variables that are shared 
    int sharedMemory = shmget(IPC_PRIVATE, sizeof(Counters), 0666 | IPC_CREAT);
    //Error checking:
    if (sharedMemory == -1)
    {
        cout << "ERROR IN SHMGET" << endl;
        cout << errno << endl;
    }

    count = (Counters *)shmat(sharedMemory, NULL, 0);
    //Error checking:
    if (count == (Counters *)-1)
    {
        cout << "ERROR IN SHMAT" << endl;
        cout << errno << endl;
    }
    //As we have specified (in lab and in instructions), we should start by masking/blocking SIGUSER1 and SIGUSER2 in the main function:
    block_signal(SIGUSR1);
    block_signal(SIGUSR2);

    //Setting up the mutexes for both
    pthread_mutexattr_t attributeOne;
    pthread_mutexattr_init(&attributeOne);
    pthread_mutexattr_setpshared(&attributeOne, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(count->mutexOne), &attributeOne);

    pthread_mutexattr_t attributeTwo;
    pthread_mutexattr_init(&attributeTwo);
    pthread_mutexattr_setpshared(&attributeTwo, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(count->mutexTwo), &attributeTwo);

    // Creating the processes. I am doing this a little bit differently than the instructions have laid out becuase I could not figure out how to do this the same way the instructions describe.
    //I instead have 8 sub-processes. However, I am still forking and using the child process as specified:
    pid_t procType[8];
    for (int i = 0; i <= SUB_PROCESSES; i++)
    {
        //Message for terminal so that I know that it is functioning correctly
        cout << "Value of I in Parent = " << i << endl;
        procType[i] = fork();
        //Error Checking
        if (procType[i] == -1)
        {
            cout << "ERROR FORKING" << endl;
        }

        //If the child:
        if (procType[i] == 0)
        {
            //Message for terminal so that I know that it is functioning correctly
            cout << "Value of I in Child = " << i << " " << getppid() << " -> " << getpid() << endl;
            if (i < 2) //When i is less than 2, I have it set up so that it will create the sigUserOne handlers at this point
            {
                cout << "Creating sigOne Handlers " << procType[i] << endl;
                signalUpdater(1);
            }
            else if (i >= 2 && i < 4) //when i is 2 or 3, I have it to create the signal handlers for sigUser2
            {
                // Creating 2 signal two handlers:
                cout << "Creating sigTwo Handlers " << procType[i] << endl;
                signalUpdater(2);
            }
            else if (i == 4) //when i is 4, we create the reporter, as specified in instructions. 
            {
                cout << "Creating reporter" << endl;
                reporter(1);
            }
            else if (i > 4 && i < 8) //if i is 5,6, or 7, we create the signal generator (the automatic processing)
            {
                cout << "Creating sigGen" << endl;
                generator();
            }
            if (i == 7)
            {
                waitpid(procType[i], NULL, 0);
            }
        }
        //Else, we are in the parent:
        else
        {

            if (i == SUB_PROCESSES)
            {

                if (TIME_TO_RUN == -1)
                {
                    while (true)
                    {
                        if (maxCountReached()) //if we reached max:
                        {

                            for (int y = 0; y < SUB_PROCESSES; y++)
                            {
                                // If we reach the max count that we set, kill all child processes.
                                cout << "Attempting to kill process " << y << " " << procType[y] << endl;
                                kill(procType[y], SIGINT);
                            }

                            sleep(2);

                            cout << count->sentSIGUSR1 << " " << count->receivedSIGUSR1 << endl;
                            cout << count->sentSIGUSR2 << " " << count->receivedSIGUSR2 << endl;

                            // Freeing shared memory in parent and exiting:
                            int detatchedVal = shmdt(count);
                            if (detatchedVal == -1)
                            {
                                cout << "Failed to detatch shared memory." << endl;
                            }
                            exit(0); //exiting when successful
                        }
                    }
                }
                else
                {
                    cout << "MADE IT TO THIS POINT" << endl;
                    sleep(TIME_TO_RUN);
                    cout << "MAIN FUNCTION PID " << getpid() << endl;

                    for (int y = 0; y < SUB_PROCESSES; y++)
                    {
                        // If max count reached kill all child processes.
                        cout << "Attempting to kill process " << y << " " << procType[y] << endl;
                        kill(procType[y], SIGINT);
                    }

                    sleep(2);
                    cout << count->sentSIGUSR1 << " " << count->receivedSIGUSR1 << endl;
                    cout << count->sentSIGUSR2 << " " << count->receivedSIGUSR2 << endl;

                    // Freeing shared memory in parent and exiting
                    int detatchedVal = shmdt(count);
                    if (detatchedVal == -1)
                    {
                        cout << "Failed to detatch shared memory." << endl;
                    }

                    exit(0); //exiting when successful
                }
            }

            sleep(1);
        }
    }

    return 0;
}
/**
 * @brief This is my main signalHandler function. I modified the provided signalHandler function layout because I am following an online tutorial that does this in C++ rather than C
 * Handling the SIGINT signal
 * @param signal the given signal passed in
 */
void signalHandler(int signal)
{
    //If the signal is SIGINT, kill process in signal handler
    if (signal == SIGINT)
    {
        cout << "Killing " << getpid() << " in signalHandler" << endl;
        int detatchedVal = shmdt(count);
        if (detatchedVal == -1) //error checking
        {
            cout << "Failed to detatch shared memory." << endl;
        }
        exit(0);
    }
    //Following provided layout, so if signal is SIGUSER1, Print out that sigUSER1 was received
    if (signal == SIGUSR1)
    {
        //I will do the locking and unlocking of the lock for signalCounter separetely (signalUpdater function)!
        cout << "\t\t\tReceived SIGUSER1 in signalHandler" << endl;
    }
    //Following provided layout same as SIGUSER!
    else if (signal == SIGUSR2)
    {
        //I will do the locking and unlocking of the lock for signalCounter separetely!
        cout << "\t\t\tReceived SIGUSER2 in signalHandler" << endl;
    }
}

/**
 * @brief my function for blocking/unblocking signals as well as printing messages to the terminal to indicate status of signal:
 * waiting, sending, releasing, receiving...
 * 
 * @param value value is 1 for SIGUSER1 and 2 for SIGUSER2
 */
void signalUpdater(int value)
{

    //Printing to terminal SignalUpdater next to process pid
    cout << "SignalUpdater " << getppid() << " -> " << getpid() << endl;

    unblock_signal(SIGINT); //unblock the signal
    signal(SIGINT, signalHandler); //signal

    //If inputted param is 1, unblock and signal SIGUSER1
    if (value == 1)
    {
        unblock_signal(SIGUSR1);
        signal(SIGUSR1, signalHandler);
    }
    
    //If inputted param is 1, unblock and signal SIGUSER1
    if (value == 2)
    {
        unblock_signal(SIGUSR2);
        signal(SIGUSR2, signalHandler);
    }
    
    //Same as provided layout, we run while true:
    while (true)
    {

        pause();

        if (value == 1) //If SIGUSER1, print following messages and lock the lock, updated count for the signal and then unlock the lock:
        {
            cout << "Received SIGUSR1 " << getppid() << " -> " << getpid() << endl;
            //As described in the instructions, I place a mutex to lock the shared memory and then updated receivedSIGUSER1 count
            pthread_mutex_lock(&count->mutexOne);
            count->receivedSIGUSR1++;
            pthread_mutex_unlock(&count->mutexOne); //now updated, we unlock the lock
        }
        if (value == 2) //If SIGUSER2, print following messages and lock the lock, updated count for the signal and then unlock the lock:
        {
            cout << "Received SIGUSR2 " << getppid() << " -> " << getpid() << endl;
            //As described in the instructions, I place a mutex to lock the shared memory and then updated receivedSIGUSER1 count
            pthread_mutex_lock(&count->mutexTwo);
            count->receivedSIGUSR2++;
            pthread_mutex_unlock(&count->mutexTwo); //now updated, we unlock the lock
        }
    }

    return;
}

/**
 * @brief function for handling reporter/reporting:
 * 
 * @param sig the signal
 */
void reporterHandler(int sig)
{

    cout << "ReporterHandler " << getppid() << " -> " << getpid() << endl;
    //same setup from above for error checking and killing:
    if (sig == SIGINT)
    {
        cout << "Killing " << getppid() << " -> " << getpid() << " in reporterHandler" << endl;
        //Freeing shared memory:
        int detatchedVal = shmdt(count);
        if (detatchedVal == -1)
        {
            cout << "Failed to detatch shared memory." << endl;
        }
        exit(0);
    }

    //If signal is SIGUSER1:
    if (sig == SIGUSR1)
    {
        //get the time and print out the the sum of time for that signal to terminal:
        clock_gettime(CLOCK_REALTIME, &timeSIGUSR1); //Note: I could not use the same time functions provided previously because I decided to do this lab in C++
        timeSummedSIGUSER1 += timeSIGUSR1.tv_sec + timeSIGUSR1.tv_nsec;
        timeSummedSIGUSER1 /= 1000000000L;
        cout << "timeSum " << timeSummedSIGUSER1 << endl;
    }
    //Otherwise, basically do same thing but for SIGUSER2
    else if (sig == SIGUSR2)
    {
        clock_gettime(CLOCK_REALTIME, &timeSIGUSR2);
        timeSummedSIGUSER2 += timeSIGUSR2.tv_sec + timeSIGUSR2.tv_nsec;
        timeSummedSIGUSER2 /= 1000000000L;
    }
}
/**
 * @brief This is a separate function from the reporterHandler which focuses on reporting to the log file
 * 
 * @param value value for signal
 */
void reporter(int value)
{
    
    cout << "Reporter " << getppid() << " -> " << getpid() << endl;

    //unblocking singals and signaling:
    unblock_signal(SIGUSR1);
    unblock_signal(SIGUSR2);
    unblock_signal(SIGINT);
    signal(SIGINT, reporterHandler);
    signal(SIGUSR1, reporterHandler);
    signal(SIGUSR2, reporterHandler);

    //while true, we are going to have the reporting thread in the receiving process monitor the receive signal counter
    //When an aggregate of 20 signals have arrived, I log the signal type, receive time, the receiving thread ID for those signals, and the average time between signals to a file
    while (true)
    {

        pause();

        ofstream log;

        //Found online for getting current time in C++
        auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());


        //open a log file and call it logP.txt to hold the info generated for multi-processing with signals:
        log.open("logP.txt", ios::app);
        if (!log.is_open()) //error checking
        {
            cout << "Couldnt open file correctly" << endl;
        }

        concurrentProcesses++; //increment concurrentProcesses
        //if we have aggregated 20 signals, we log the information specified earlier:
        if (concurrentProcesses == 20)
        {
            //As specified in instructions, we create mutex locks because of the shared access to count
            //That is, we are providing each thread exclusive access to each shared structure
            pthread_mutex_lock(&count->mutexOne);
            pthread_mutex_lock(&count->mutexTwo);

            //basically resetting the process amount
            concurrentProcesses = 0;

            //getting the average time for both SIGUSER1 and SIGUSER2 by dividing by 20 (number of signals)
            double averageTimeSig1 = timeSummedSIGUSER1 / 20.0;
            double averageTimeSig2 = timeSummedSIGUSER2 / 20.0;

            //Adding the information to the log file:
            log << "SIGUSR1 |"
                << " | Sent " << count->sentSIGUSR1 << " Received " << count->receivedSIGUSR1 << " | "
                << "Current Time " << ctime(&currentTime) << " | Avg Time Between Signals " << averageTimeSig1 << endl;
            log << "SIGUSR2 |"
                << "| Sent " << count->sentSIGUSR2 << " Received " << count->receivedSIGUSR2 << " | "
                << "Current Time " << ctime(&currentTime) << " | Avg Time Between Signals " << averageTimeSig2 << endl;
            
            //resetting average time and timeSum since we are done with them now:
            averageTimeSig1 = 0;
            averageTimeSig2 = 0;

            timeSummedSIGUSER1 = 0;
            timeSummedSIGUSER2 = 0;

            //We are done accessing shared state, so we unlock the locks:
            pthread_mutex_unlock(&count->mutexOne);
            pthread_mutex_unlock(&count->mutexTwo);
        }
    }
}

/**
 * @brief my function for handling the generator:
 * 
 * @param signal the signal inputted as a parameter
 */
void generatorHandler(int signal)
{
    //printed to terminal for clarity and error checking:
    cout << "Generator Handler " << getppid() << " -> " << getpid() << endl;

    //same error checking and killing if statement as before:
    if (signal == SIGINT)
    {
        cout << "Killing " << getppid() << " -> " << getpid() << " in generatorHandler" << endl;
        int detatchedVal = shmdt(count);
        if (detatchedVal == -1)
        {
            cout << "Failed to detatch shared memory." << endl;
        }
        exit(0);
    }
}

/**
 * @brief generator function
 * 
 */
void generator()
{
    //printing to terminal for clarity and error checking:
    cout << "Generator " << getppid() << " -> " << getpid() << endl;

    signal(SIGINT, generatorHandler); //send signal to generatorHandler

    srand(time(NULL)); //random time

    int x = 0;
    //We need to randomly send signals for either SIGUSER1 or SIGUSER2
    //To do this, I am creating a random number and using a random number generator that goes between 0 and 1
    //If it is less than .5, we send SIGUSER1
    //If it is equal to or above .5, we send SIGUSER2
    //There may be other options for random generation, but this method seems to work well
    while (true)
    {
        x++;
        double theRandomNum = randGenerator(0.0, 1.0);
        if (theRandomNum < 0.5)
        {
            cout << "Sent SIGUSR1 " << getppid() << " -> " << getpid() << endl;
            pthread_mutex_lock(&count->mutexOne); //creating lock and locking because of shared access 
            count->sentSIGUSR1++; //incrementing
            pthread_mutex_unlock(&count->mutexOne); //unlocking lock
            kill(0, SIGUSR1); //send kill
        }
        else if (theRandomNum >= 0.5)
        {
            //Basically same process as above but for SIGUSER2:
            cout << "Sent SIGUSR2 " << getppid() << " -> " << getpid() << endl;
            pthread_mutex_lock(&count->mutexTwo);
            count->sentSIGUSR2++;
            pthread_mutex_unlock(&count->mutexTwo);
            kill(0, SIGUSR2);
        }

        //As stated in the instructions, each repetition through the signal generating loop is going to have a randomly generating time delay ranging from .01 to .1 seconds before the next reperition
        struct timespec time_delay;
        time_delay.tv_sec = 0;
        time_delay.tv_nsec = randGenerator(0.01, 0.1) * 1000000000L;
        nanosleep(&time_delay, NULL); //we use nanosleep to create such a delay
    }
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
 * @param signal the signal inputted as param
 */
void block_signal(int signal)
{
    sigset_t sigset;
    sigemptyset(&sigset); //initializing the set to empty
    sigaddset(&sigset, signal); //adding SIGINT to the set 
    int sigReturn = sigprocmask(SIG_BLOCK, &sigset, NULL); //modifying the mask
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
    int sigReturn = sigprocmask(SIG_UNBLOCK, &sigset, NULL);
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
