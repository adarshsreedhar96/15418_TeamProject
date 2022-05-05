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
    const int numOfThreads = 4;
    int numberOfTasks = 45;
    RandomSleep randomsleep;
    typedef TaskNumArgs *TNArgs;
    TNArgs *args = (TNArgs *)malloc(sizeof(TaskNumArgs *) * numberOfTasks);
    randomsleep.setInput(numOfThreads * 5, numOfThreads);
    randomsleep.getTasks(args, numberOfTasks);
#if CENTRALIZED_QUEUE
    threadPool threadPool(num_of_threads);
    threadPool.submit(&RandomSleep::workerTask, args, numberOfTasks);
#endif
#if PERTHREAD_QUEUE
    threadPool_PerThread threadPool(numOfThreads, true, STEALONETASK);
    threadPool.submit(&RandomSleep::workerTask, args, numberOfTasks);
#endif
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %f\n", std::chrono::duration<double, std::milli>(end_time - start_time).count());
    // printf("time diff: %d\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time-start_time).count());
}