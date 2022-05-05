#include <stdio.h>
#include <unistd.h>
#include "matrixmul.cpp"
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

    const int numOfThreads = 8;
    int numberOfTasks = 100;

    MatMul matMul;
    MatMul::matrixmul **args;
    int size = 500;
    int mat1[size * size];
    int mat2[size * size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            mat1[i * size + j] = i + j;
            mat2[i * size + j] = i + j;
        }
    }

    args = (MatMul::matrixmul **)malloc(sizeof(MatMul::matrixmul *) * numberOfTasks);
    matMul.setInput(mat1, mat2, size);
    matMul.getTasks(args, numberOfTasks);
#if CENTRALIZED
    threadPool threadPool(numOfThreads);
#else
    threadPool_PerThread threadPool(numOfThreads, true, STEALHALFTASKS);
#endif
    // int *priorities = (int *)malloc(sizeof(int) * numberOfTasks);
    // threadPool.submit(&MatMul::workerTask, args, priorities, numberOfTasks);
    threadPool.submit(&MatMul::workerTask, args, numberOfTasks);
    threadPool.dispatch();
    threadPool.clearTasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int *temp = args[0]->output + ((i)*args[0]->size) + j;
            printf("%d ", *temp);
        }
        printf("\n");
    }
}