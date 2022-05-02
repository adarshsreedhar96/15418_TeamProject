//#include "threadpool_centralized.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "matrixmul.cpp"
using namespace std;

int main()
{

    const int numOfThreads = 8;
    //
    //void *args;
    MatMul matMul;
    MatMul::matrixmul* args = (MatMul::matrixmul *)malloc(sizeof(MatMul::matrixmul *) * numOfThreads);
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

    matMul.setInput(mat1, mat2, 16);
    matMul.getTasks(&args, numOfThreads);
    threadPool_PerThread threadPool(numOfThreads);
    // threadPool.submit(&printEmpty, numOfThreads);
    threadPool.submit(&MatMul::workerTask, &args, numOfThreads);
    threadPool.dispatch();
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            int* temp = args[0].output+((i-1)*args[0].size)+j;
            printf("[%d][%d]: %d\n", i, j, *temp);
        }
    }
}