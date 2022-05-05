#include <stdio.h>
#include <unistd.h>
#include "randomsleep.cpp"
#include <iostream>
#include <chrono>
#include <thread>

#define PERTHREAD_QUEUE 1
#define CENTRALIZED_QUEUE 0

#if CENTRALIZED_QUEUE
#include "threadpool_centralized.h"
#endif
#if PERTHREAD_QUEUE
#include "threadpool_perthread.h"
#endif

#define WITHOUTARGS 1
#define WITHARGS 0

using namespace std;

int main()
{
    #if CENTRALIZED_QUEUE
    threadPool threadPool(num_of_threads);
    threadPool.submit(&printEmpty, num_of_threads);
    threadPool.dispatch();
    threadPool.clearTasks();
    #endif
    #if PERTHREAD_QUEUE
    const int numOfThreads = std::thread::hardware_concurrency();
    int numberOfTasks = numOfThreads * 5;
    RandomSleep randomsleep;
    typedef TaskNumArgs* TNArgs;
    TNArgs* args = (TNArgs*) malloc(sizeof(TaskNumArgs*)*numberOfTasks);

    randomsleep.setInput(numOfThreads*5, numOfThreads);

    randomsleep.getTasks(args, numberOfTasks);
    
    threadPool_PerThread threadPool(numOfThreads, true, STEALALLTASKS);
    threadPool.submit(&RandomSleep::workerTask, args, numberOfTasks);
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %f\n", std::chrono::duration<double, std::milli>(end_time-start_time).count());
    //printf("time diff: %d\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time-start_time).count());
    #endif
}