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

using namespace std;

int main()
{

    const int numOfThreads = 2;
    int numberOfTasks = 4;
    Search searchObj;
    Search::search **args;
    std::string text = "I am a test string and I don't know what I am doing cause CMU is killing me and I am dead inside and this is a ridiculous test";

    args = (Search::search **)malloc(sizeof(Search::search *) * numberOfTasks);
    printf("Setting input\n");
    searchObj.setInput(text, "ridiculous");
    printf("Getting tasks\n");
    searchObj.getTasks(args, numberOfTasks);
    threadPool_priority threadPool(numOfThreads, &Search::workerTask);
    // threadPool_PerThread threadPool(numOfThreads);
    int *priority = (int *)malloc(sizeof(int) * numberOfTasks);
    priority[0] = 10;
    priority[1] = 9;
    priority[2] = 8;
    priority[3] = 7;
    printf("Submitting tasks\n");
    threadPool.submit(&Search::workerTask, args, priority, numberOfTasks);
    threadPool.dispatch();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    printf("was i found? %d\n", args[3]->found);
}