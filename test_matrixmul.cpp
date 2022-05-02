//#include "threadpool_centralized.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "matrixmul.cpp"
using namespace std;

int main()
{

    const int numOfThreads = std::thread::hardware_concurrency();
    //
    void *args;
    int mat1[16] = {
        1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        4, 4, 4, 4};

    int mat2[16] = {
        1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        4, 4, 4, 4};

    MatMul matMul = new MatMul;
    matMul.setInput(&mat1, &mat2, 16);
    matMul.getTasks(args, numOfThreads);
    threadPool_PerThread threadPool(numOfThreads);
    // threadPool.submit(&printEmpty, numOfThreads);
    threadPool.submit(&matMul.workerTask, args, numOfThreads);
    threadPool.dispatch();
}