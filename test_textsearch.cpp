#include <stdio.h>
#include <unistd.h>
#include "textsearch.cpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <cstdlib>
#include "random.h"

#define PERTHREAD_QUEUE 0
#define PRIORITY_QUEUE 1

#if PRIORITY_QUEUE
#include "threadpool_priority.h"
#endif
#if PERTHREAD_QUEUE
#include "threadpool_perthread.h"
#endif

using namespace std;

int main(int argc, char **argv)
{
    const int numOfThreads = atoi(argv[1]);
    int numberOfTasks = atoi(argv[2]);
    Search searchObj;
    Search::search **args;
    std::string text = txt;

    args = (Search::search **)malloc(sizeof(Search::search *) * numberOfTasks);
    printf("Setting input\n");
    searchObj.setInput(text, "scary");
    printf("Getting tasks\n");
    searchObj.getTasks(args, numberOfTasks);
    bool result;
    int *sectionpriority = (int *)malloc(sizeof(int) * numberOfTasks);
    for (int i = 0; i < numberOfTasks; i++)
    {
        sectionpriority[i] = i;
        // sectionpriority[i] = numberOfTasks - i;
    }
    int *priority = searchObj.getPriority(numberOfTasks, 4, sectionpriority);
    printf("Submitting tasks\n");
#if PRIORITY_QUEUE
    threadPool_priority threadPool(numOfThreads, &Search::workerTask, &result);
    threadPool.submit(&Search::workerTask, args, priority, numberOfTasks);
#endif
#if PERTHREAD_QUEUE
    threadPool_PerThread threadPool(numOfThreads, false, STEALALLTASKS, STEALRANDOMTASK, &result);
    threadPool.submit(&Search::workerTask, args, numberOfTasks);
#endif
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %f\n", std::chrono::duration<double, std::milli>(end_time - start_time).count());
    printf("was i found? %d\n", result);
}