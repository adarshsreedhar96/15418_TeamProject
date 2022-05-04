//#include "threadpool_centralized.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "mandelbrot.h"
using namespace std;

void printEmpty(){
    printf("helloworld called by thread: %d\n", std::this_thread::get_id());
}

int main(){

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
    // 
    typedef WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numOfThreads);
    for (int i = 0; i < numOfThreads; i++) {
        args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
    }

    //WorkerArgs* args = (WorkerArgs*) malloc(sizeof(WorkerArgs)*numOfThreads);
    int width = 600;
    int height = 600;
    int *output = new int[width * height];

    for (int i = 0; i < numOfThreads; i++) {
        args[i]->threadId = i;
        // TODO: Set thread arguments here
        // assigning all values
        args[i]->x0 = -2.167;
        args[i]->y0 = 1.167;
        args[i]->x1 = -1;
        args[i]->y1 = 1;
        args[i]->height = height;
        args[i]->width  = width;
        args[i]->maxIterations = 100;
        args[i]->output = output;
        args[i]->numThreads= numOfThreads;
        args[i]->startRow  = i*(height/numOfThreads);
        if(i==(numOfThreads-1)){
            args[i]->totalRows  = height-args[i]->startRow;
        } else {
            args[i]->totalRows = height/numOfThreads;
        }
    }
    threadPool_PerThread threadPool(numOfThreads);
    //threadPool.submit(&printEmpty, numOfThreads);
    threadPool.submit(&workerThreadStart, args, numOfThreads);
    threadPool.dispatch();
    #endif
    return 0;
}