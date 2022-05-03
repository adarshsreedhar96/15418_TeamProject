//#include "threadpool_centralized.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "mandelbrot.h"
#include <stdlib.h>
#include <cstring>
#include <getopt.h>

using namespace std;

void printEmpty(){
    printf("helloworld called by thread: %d\n", std::this_thread::get_id());
}

int main(int argc, char **argv) {
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
    memset(output_thread, 0, width * height * sizeof(int));
    typedef WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numOfThreads);
    for (int i = 0; i < numOfThreads; i++) {
        args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
    }
    for (int i = 0; i < numOfThreads; i++) {
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
        args[i]->totalRows = height/numOfThreads;
        args[i]->startRow  = i*args[i]->totalRows;
        if((i+1)==numOfThreads){
            args[i]->totalRows  = height-args[i]->startRow;
        }
    }
        threadPool_PerThread threadPool(numOfThreads);
        threadPool.submit(&workerThreadStart, args, numOfThreads);
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
}