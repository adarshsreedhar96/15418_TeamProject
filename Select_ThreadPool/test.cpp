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
    const int width = 600;
    // Height of the canvas
    const int height = 600;
    // Height of the actual image.  Make smaller to detect overrunning array
    const int maxIterations = 256;
    const int numOfThreads = std::thread::hardware_concurrency();

    float x0 = -2.167;
    float x1 = 1.167;
    float y0 = -1;
    float y1 = 1;

    // Support VIEWCNT views
    float scaleValues[VIEWCNT] = {0.01f, 1.0f,  0.015f, 0.02f, 0.02f, 0.02f, 0.002f};
    float shiftXs[VIEWCNT] = {0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f, -1.4f};
    float shiftYs[VIEWCNT] = {0.0f, 0.0f, 0.30f, 0.05f, 0.73f, 0.0f, 0.0f};

    int viewIndex = 3;
    // // End parsing of commandline options
    float scaleValue = scaleValues[viewIndex];
    float shiftX = shiftXs[viewIndex];
    float shiftY = shiftYs[viewIndex];
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    int *output_serial = new int[width * height];
    int *output_thread = new int[width * height];

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
        sleep(5);

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

/*
int main(int argc, char **argv){
    #if 0
    //int num_of_threads = std::thread::hardware_concurrency();
    int num_of_threads = 16;
    threadPool threadPool(num_of_threads);
    for(int i=0;i<num_of_threads;i++){
        threadPool.submit(&printEmpty);
    }
    threadPool.dispatch();
    #endif
    #if 1
    const int numOfThreads = std::thread::hardware_concurrency();
    // parse cmd line
    int viewIndex = 0;
    // int numThreads = std::thread::hardware_concurrency();
    // parse_cmd_options(argc, argv, &numThreads, &viewIndex);
    // const int numOfThreads = numThreads;
    
    // serial part
    float scaleValue = scaleValues[viewIndex];
    float shiftX = shiftXs[viewIndex];
    float shiftY = shiftYs[viewIndex];
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    #if 1
    int *output_serial = new int[width * height];
    memset(output_serial, 0, width * height * sizeof(int));
    mandelbrotSerial(x0, y0, x1, y1, width, iheight, 0, iheight, maxIterations, output_serial);
    #endif

    // multithreading part
    typedef WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numOfThreads);
    for (int i = 0; i < numOfThreads; i++) {
        args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
    }

    int *output_thread = new int[width * height];
    memset(output_thread, 0, width * height * sizeof(int));

    for (int i = 0; i < numThreads; i++) {
        args[i]->threadId = i;
        args[i]->x0 = x0;
        args[i]->y0 = y0;
        args[i]->x1 = x1;
        args[i]->y1 = y1;
        args[i]->height = height;
        args[i]->width  = width;
        args[i]->maxIterations = maxIterations;
        args[i]->output = output_thread;
        args[i]->numThreads= numThreads;
        args[i]->totalRows = height/numThreads;
        args[i]->totalRows = height;
        args[i]->startRow  = i;
    }
    threadPool_PerThread threadPool(numOfThreads);
    //threadPool.submit(&printEmpty, numOfThreads);
    threadPool.submit(&workerThreadStart, args, numOfThreads);
    threadPool.dispatch();
    sleep(10);
    if (!verifyResult(output_serial, output_thread, width, height)) {
        printf("ERROR : Output from threads does not match serial output\n");

        delete[] output_serial;
        delete[] output_thread;

        return 1;
    } else {
        printf("results match\n");
    }
    #endif
    return 0;
}
*/