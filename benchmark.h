#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <functional>

using namespace std;

class Benchmark
{

public:
    // method to set certain input values for this benchmark
    void setInputs() {}

    // method to divide the workload into given number of tasks
    template <typename T>
    void getTasks(T **args, int numberOfTasks) {}

    // worker method which takes a set of arguments in the form of a struct pointer
    void workerTask(void *threadArgs) {}
};