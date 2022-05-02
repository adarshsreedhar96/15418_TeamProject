#include "benchmark.h"

using namespace std;
class MatMul : Benchmark
{

private:
    int *mat1;
    int *mat2;
    int size;
    struct matrixmul
    {
        int *mat1;
        int *mat2;
        int *output;
        int size;
        int start_idx;
        int end_idx;
    };

public:
    void setInput(int *mat1, int *mat2, int n)
    {
        this->mat1 = mat1;
        this->mat2 = mat2;
        this->size = n;
    }

    void workerTask(void *threadArgs)
    {
        matrixmul *args = static_cast<matrixmul *>(threadArgs);
        for (int i = args->start_idx; i < args->end_idx; i++)
        {
            for (int j = args->start_idx; j < args->end_idx; j++)
            {
                int *res = args->output + (args->size * (i - 1)) + j;
                *res = 0;
                for (int k = args->start_idx; k < args->end_idx; k++)
                {
                    *res += *(mat1 + (args->size * (i - 1) + k)) * *(mat2 + (args->size * (k - 1) + j));
                }
            }
        }
    }

    void getTasks(matrixmul **args, int numberOfTasks)
    {
        int *output = (int *)malloc(this->size * this->size * sizeof(int));
        *args = (matrixmul *)malloc(sizeof(matrixmul *) * numberOfTasks);
        // *args = static_cast<matrixmul *>(threadArgs);
        for (int i = 0; i < numberOfTasks; i++)
        {
            args[i] = (matrixmul *)malloc(sizeof(matrixmul));
            args[i]->mat1 = this->mat1;
            args[i]->mat2 = this->mat2;
            args[i]->size = this->size;
            args[i]->output = output;
            args[i]->start_idx = i * (size / numberOfTasks);
            args[i]->end_idx = (i + 1) * (size / numberOfTasks);
        }
        args[numberOfTasks - 1]->end_idx = size;
    }
};