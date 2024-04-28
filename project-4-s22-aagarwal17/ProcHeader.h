
/**
 * @file ProcHeader.h
 * @author Arun Agarwal (tuk82261@temple.edu)
 * @brief header file for ProcSignaling.cpp
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//Packages being used:
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <errno.h>

//Shared Memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <vector>

//struct for Counters:
typedef struct Counters
{
    int sentSIGUSR1 = 0;
    int sentSIGUSR2 = 0;
    int receivedSIGUSR1 = 0;
    int receivedSIGUSR2 = 0;
    pthread_mutex_t mutexOne;
    pthread_mutex_t mutexTwo;
} Counters;

using namespace std;

//function headers defined:
void signalHandler(int signal);
void signalUpdater(int value);
void reporterHandler(int sig);
void reporter(int value);
void generatorHandler(int signal);
void generator();
double randGenerator(double lowerBound, double upperBound);
void unblock_signal(int signal);
void block_signal(int signal);
bool maxCountReached();