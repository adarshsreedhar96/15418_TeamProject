#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <getopt.h>
#include <time.h>

#define PERTHREAD_QUEUE 1
#define CENTRALIZED_QUEUE 0

#if CENTRALIZED_QUEUE
#include "threadpool_centralized.h"
#endif
#if PERTHREAD_QUEUE
#include "threadpool_perthread.h"
#endif

//#include "mandelbrot.h"
#include "variablesleep.h"

#define WITHOUTARGS 1
#define WITHARGS 0

using namespace std;

void printEmpty(){
    //printf("helloworld called by thread: %d\n", std::this_thread::get_id());
    srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    int timeToSleep = rand() % 10;
    printf("thread_id: %d sleep for %d\n", std::this_thread::get_id(), timeToSleep);
    sleep(timeToSleep);
}

int main(int argc, char **argv) {
    #if WITHOUTARGS
    int num_of_threads = std::thread::hardware_concurrency();
    int numOfTasks = num_of_threads * 3;
    #if CENTRALIZED_QUEUE
    threadPool threadPool(num_of_threads);
    threadPool.submit(&printEmpty, num_of_threads);
    threadPool.dispatch();
    threadPool.clearTasks();
    #endif
    #if PERTHREAD_QUEUE

    threadPool_PerThread threadPool(num_of_threads);
    typedef TaskNumArgs* TNArgs;
    TNArgs* args = (TNArgs*) malloc(sizeof(TaskNumArgs*)*numOfTasks);
    for (int i = 0; i < numOfTasks; i++) {
        args[i] =  (TaskNumArgs*) malloc(sizeof(TaskNumArgs));
    }
    for (int i = 0; i < numOfTasks; i++) {
            args[i]->taskNum = i;
    }
    threadPool.submit(&printTaskNum, args, numOfTasks);
    threadPool.dispatch();
    threadPool.clearTasks();
    #endif

    #endif

    #if WITHARGS
    // change this as required
    viewIndex = 6;
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    //
    // Run the serial implementation.  Run the code numRuns times and
    // take the minimum to get a good estimate.
    //
    memset(output_serial, 0, width * height * sizeof(int));
    mandelbrotSerial(x0, y0, x1, y1, width, height, 0, height, maxIterations, output_serial);

    //
    // Run the threaded version
    //
    //const int numOfThreads = std::thread::hardware_concurrency();
    int numOfTasks = numOfThreads * 5;
    memset(output_thread, 0, width * height * sizeof(int));
    typedef WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numOfTasks);
    for (int i = 0; i < numOfTasks; i++) {
        args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
    }
    for (int i = 0; i < numOfTasks; i++) {
        args[i]->threadId = i;
        args[i]->x0 = x0;
        args[i]->y0 = y0;
        args[i]->x1 = x1;
        args[i]->y1 = y1;
        args[i]->height = height;
        args[i]->width  = width;
        args[i]->maxIterations = maxIterations;
        args[i]->output = output_thread;
        args[i]->numThreads= numOfThreads;
        args[i]->totalRows = height/numOfTasks;
        args[i]->startRow  = i*args[i]->totalRows;
        if((i+1)==numOfTasks){
            args[i]->totalRows  = height-args[i]->startRow;
        }
    }
        #if PERTHREAD_QUEUE
        threadPool_PerThread threadPool(numOfThreads);
        threadPool.submit(&workerThreadStart, args, numOfTasks);
        #endif
        #if CENTRALIZED_QUEUE
        threadPool threadPool(numOfThreads);
        threadPool.submit(&workerThreadStart, args, numOfTasks);
        #endif

        threadPool.dispatch();
        threadPool.clearTasks();

    if (!verifyResult(output_serial, output_thread, width, height)) {
        printf("ERROR : Output from threads does not match serial output\n");

        delete[] output_serial;
        delete[] output_thread;

        return 1;
    } else {
        printf("output matches\n");
        delete[] output_serial;
        delete[] output_thread;
        return 0;
    }
    #endif
}