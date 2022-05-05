//#include "threadpool_centralized.h"
// #include "threadpool_perthread.h"
#include "threadpool_priority.h"
// #include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "textsearch.cpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
    const int numOfThreads = atoi(argv[1]);
    int numberOfTasks = atoi(argv[2]);
    Search searchObj;
    Search::search **args;
    std::string text = "I am a test string and I don't know what I am doing cause CMU is killing me and I am dead inside and this is a ridiculous test";

    args = (Search::search **)malloc(sizeof(Search::search *) * numberOfTasks);
    printf("Setting input\n");
    searchObj.setInput(text, "string");
    printf("Getting tasks\n");
    searchObj.getTasks(args, numberOfTasks);
    bool result;
    threadPool_priority threadPool(numOfThreads, &Search::workerTask, &result);
    // threadPool_PerThread threadPool(numOfThreads);
    int *sectionpriority = (int *)malloc(sizeof(int) * numberOfTasks);
    sectionpriority[0] = 1;
    sectionpriority[1] = 2;
    sectionpriority[2] = 3;
    sectionpriority[3] = 4;
    int *priority = searchObj.getPriority(numberOfTasks, 4, sectionpriority);
    printf("Submitting tasks\n");
    threadPool.submit(&Search::workerTask, args, priority, numberOfTasks);
    threadPool.dispatch();
    threadPool.clearTasks();
    printf("was i found? %d\n", result);
}