#include "benchmark.h"
#include <string.h>

using namespace std;
class MatMul : Benchmark
{

private:
    int *mat1;
    int *mat2;
    int size;

public:
    typedef struct
    {
        int *mat1;
        int *mat2;
        int *output;
        int size;
        int start_idx;
        int end_idx;
    } matrixmul;
    void setInput(int *mat1arg, int *mat2arg, int n)
    {
        // this->mat1 = (int *)malloc(sizeof(int) * n * n);
        // this->mat2 = (int *)malloc(sizeof(int) * n * n);
        // memcpy(this->mat1, mat1, sizeof(int) * n * n);
        // memcpy(this->mat2, mat2, sizeof(int) * n * n);
        this->mat1 = mat1arg;
        this->mat2 = mat2arg;
        this->size = n;
    }

    static void workerTask(void *threadArgs)
    {
        printf("workerThreadStart called by thread: %d\n", std::this_thread::get_id());
        matrixmul *args = static_cast<matrixmul *>(threadArgs);
        printf("set by thread args are %d\n", args->mat1[0]);
        for (int i = 0; i < args->size; i++)
        {
            for (int j = 0; j < args->size; j++)
            {
                int linear_idx = (args->size * (i)) + j;
                int *res = args->output + linear_idx;
                *res = 0;
                for (int k = args->start_idx; k < args->end_idx; k++)
                {
                    // printf("thread args are %d,%d,%d\n", i, args->size * i + k, args->mat1[args->size * i + k]);
                    *res += *(args->mat1 + (args->size * (i) + k)) * *(args->mat2 + (args->size * (k) + j));
                }
            }
        }
    }
    template <typename T>
    void getTasks(T **args, int numberOfTasks)
    {
        int *output = (int *)malloc(this->size * this->size * sizeof(int));
        *args = (matrixmul *)malloc(sizeof(matrixmul *) * numberOfTasks);
        // *args = static_cast<matrixmul *>(threadArgs);
        for (int i = 0; i < numberOfTasks; i++)
        {

            // args[i] = (matrixmul *)malloc(sizeof(matrixmul));
            args[i]->mat1 = this->mat1;
            args[i]->mat2 = this->mat2;

            args[i]->size = this->size;
            args[i]->output = output;
            args[i]->start_idx = i * (size / numberOfTasks);
            args[i]->end_idx = (i + 1) * (size / numberOfTasks);
            printf("set by thread args are %d\n", this->mat1[2]);
        }
        args[numberOfTasks - 1]->end_idx = size;
    }
};