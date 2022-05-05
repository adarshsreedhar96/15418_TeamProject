#include <stdio.h>
#include <unistd.h>
#include "mandelbrot.cpp"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

#define CENTRALIZED 0

#if CENTRALIZED
#include "threadpool_centralized.h"
#else
#include "threadpool_perthread.h"
#endif

int main()
{
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    // run serial 
    memset(output_serial, 0, width * height * sizeof(int));
    mandelbrotSerial(x0, y0, x1, y1, width, height, 0, height, maxIterations, output_serial);


    const int numOfThreads = 4;
    int numberOfTasks = 4;
    Mandelbrot mandelbrot;

    typedef Mandelbrot::WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(Mandelbrot::WorkerArgs*)*numberOfTasks);

    mandelbrot.setInput(1, numOfThreads);

    mandelbrot.getTasks(args, numberOfTasks);
    #if CENTRALIZED
    threadPool threadPool(numOfThreads);
    #else
    threadPool_PerThread threadPool(numOfThreads, true, STEALHALFTASKS);
    #endif
    threadPool.submit(&Mandelbrot::workerTask, args, numberOfTasks);
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %d\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time-start_time).count());

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