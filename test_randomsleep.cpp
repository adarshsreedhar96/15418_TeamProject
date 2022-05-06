#include <stdio.h>
#include <unistd.h>
#include "randomsleep.cpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#define PERTHREAD_QUEUE 0
#define CENTRALIZED_QUEUE 1

#if CENTRALIZED_QUEUE
#include "threadpool_centralized.h"
#endif
#if PERTHREAD_QUEUE
#include "threadpool_perthread.h"
#endif

using namespace std;

int main(int argc, char **argv)
{
    const int numOfThreads = atoi(argv[1]);
    int numberOfTasks = atoi(argv[2]);
    RandomSleep randomsleep;
    typedef TaskNumArgs *TNArgs;
    TNArgs *args = (TNArgs *)malloc(sizeof(TaskNumArgs *) * numberOfTasks);
    randomsleep.setInput(numberOfTasks, numOfThreads);
    randomsleep.getTasks(args, numberOfTasks);
#if CENTRALIZED_QUEUE
    threadPool threadPool(numOfThreads);
    threadPool.submit(&RandomSleep::workerTask, args, numberOfTasks);
#endif
#if PERTHREAD_QUEUE
    threadPool_PerThread threadPool(numOfThreads, true, STEALALLTASKS, STEALRANDOMTASK);
    threadPool.submit(&RandomSleep::workerTask, args, numberOfTasks);
#endif
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %f\n", std::chrono::duration<double, std::milli>(end_time - start_time).count());
    // printf("time diff: %d\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time-start_time).count());
}