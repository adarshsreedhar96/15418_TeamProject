// In this file, we will create a bunch of threads at runtime, and then pass a function to it 
// for parallel computation, and measure the time taken overall, as well the parallelized parts
// The idea is to check the time difference between a thread pool (hot ready threads) v/s runtime thread creation

#include<stdio.h>
#include<thread>
#include<stdlib.h>
#include "CycleTimer.h"
using namespace std;
struct matrixArgs{
    int* matrixA;
    int* matrixB;
    int num_of_threads;
    int thread_id;
};

void* addition(void* threadArgs) {
    // typecast threadArgs, aince it is a void ptr
    matrixArgs* args = (matrixArgs*)threadArgs;
    // calculate linear index
    int start_index = (100/args->num_of_threads) * args->thread_id;
    int end_index   = std::min(((100/args->num_of_threads)) * (args->thread_id+1), 100);
    printf("hello world from thread: %d start_index: %d end_index: %d\n", args->thread_id, start_index, end_index);
    return NULL;
}
void createMatrixes(int* matrixA, int* matrixB){
    matrixA = (int*)malloc(sizeof(int) * 100);
    matrixB = (int*)malloc(sizeof(int) * 100);
}
int main(){
    double startTimeSeq = CycleTimer::currentSeconds();
    // get the size of the concurrency
    int num_threads = thread::hardware_concurrency();
    // create an array to store the threads
    pthread_t threads[num_threads];
    // get pointers to initial matrices
    int* matrixA;
    int* matrixB;
    createMatrixes(matrixA, matrixB);
    // create attribute to be sent to each thread
    matrixArgs args[num_threads];
    for(int i=0;i<num_threads;i++){
        args[i].matrixA = matrixA;
        args[i].matrixB = matrixB;
        args[i].num_of_threads = num_threads;
        args[i].thread_id = i;
    }
    double startTimeParallel = CycleTimer::currentSeconds();
    // create threads
    for(int i=1;i<num_threads;i++) {
        pthread_create(&threads[i], NULL, &addition, &args[i]);
    }
    // this if for the current thread - it should not remain idle
    addition(&args[0]);
    double endTimeParallel = CycleTimer::currentSeconds();
    double timeDiffParallel = endTimeParallel - startTimeParallel;
    // join them back
    for(int i=1;i<num_threads;i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Prallel Section Time Difference: %.4f\n", timeDiffParallel);
    double endTimeSeq = CycleTimer::currentSeconds();
    printf("Seq Section Time Difference: %.4f\n", (endTimeSeq-startTimeSeq)-timeDiffParallel);
    return 0;
}