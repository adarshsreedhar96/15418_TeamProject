#include "threadpool_centralized.h"
//#include "threadpool_priority.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "mandelbrot.cpp"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

int main()
{
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    // run serial 
    memset(output_serial, 0, width * height * sizeof(int));
    mandelbrotSerial(x0, y0, x1, y1, width, height, 0, height, maxIterations, output_serial);


    const int numOfThreads = std::thread::hardware_concurrency();
    int numberOfTasks = numOfThreads * 5;
    Mandelbrot mandelbrot;

    typedef Mandelbrot::WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(Mandelbrot::WorkerArgs*)*numberOfTasks);

    mandelbrot.setInput(6, numOfThreads);

    mandelbrot.getTasks(args, numberOfTasks);
    
    threadPool threadPool(numOfThreads);
    threadPool.submit(&Mandelbrot::workerTask, args, numberOfTasks);
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