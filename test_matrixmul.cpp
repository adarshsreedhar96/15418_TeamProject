#include <stdio.h>
#include <unistd.h>
#include "matrixmul.cpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

#define CENTRALIZED 1

#if CENTRALIZED
#include "threadpool_centralized.h"
#else
#include "threadpool_perthread.h"
#endif

int main(int argc, char **argv)
{
    const int numOfThreads = atoi(argv[1]);
    int numberOfTasks = atoi(argv[2]);

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
    threadPool_PerThread threadPool(numOfThreads, true, STEALHALFTASKS, STEALRANDOMTASK);
#endif
    threadPool.submit(&MatMul::workerTask, args, numberOfTasks);
    auto start_time = std::chrono::high_resolution_clock::now();
    threadPool.dispatch();
    threadPool.clearTasks();
    auto end_time = std::chrono::high_resolution_clock::now();
    printf("time diff: %d\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());
    // for (int i = 0; i < size; i++)
    // {
    //     for (int j = 0; j < size; j++)
    //     {
    //         int *temp = args[0]->output + ((i)*args[0]->size) + j;
    //         printf("%d ", *temp);
    //     }
    //     printf("\n");
    // }
}