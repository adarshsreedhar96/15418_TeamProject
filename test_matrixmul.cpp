//#include "threadpool_centralized.h"
//#include "threadpool_priority.h"
#include "threadpool_perthread.h"
//#include "threadpool_test.h"
#include <stdio.h>
#include <unistd.h>
#include "matrixmul.cpp"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

int main()
{

    const int numOfThreads = 8;
    int numberOfTasks = 32;
    MatMul matMul;
    MatMul::matrixmul **args;
    int mat1[100] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

    int mat2[100] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

    int size = 10;
    args = (MatMul::matrixmul **)malloc(sizeof(MatMul::matrixmul *) * numberOfTasks);
    matMul.setInput(mat1, mat2, size);
    matMul.getTasks(args, numberOfTasks);
    threadPool_PerThread threadPool(numOfThreads, true, STEALHALFTASKS);
    //int *priorities = (int *)malloc(sizeof(int) * numberOfTasks);
    //threadPool.submit(&MatMul::workerTask, args, priorities, numberOfTasks);
    threadPool.submit(&MatMul::workerTask, args, numberOfTasks);
    threadPool.dispatch();
    threadPool.clearTasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int *temp = args[0]->output + ((i)*args[0]->size) + j;
            printf("[%d][%d]: %d\n", i, j, *temp);
        }
    }
}