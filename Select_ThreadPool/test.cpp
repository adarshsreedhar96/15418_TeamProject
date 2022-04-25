//#include "threadpool_centralized.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
using namespace std;

// extern void mandelbrotThread(int numThreads, float x0, float y0, float x1,
//                              float y1, int width, int height, int maxIterations,
//                              int output[]);

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
    threadPool_PerThread threadPool(std::thread::hardware_concurrency()*5);
    threadPool.submit(&printEmpty);
    threadPool.dispatch();
    //sleep(2);
    #endif
    return 0;
}