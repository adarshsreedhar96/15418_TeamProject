#include <stdio.h>
#include <stdlib.h>
#include <thread>

using namespace std;

class Benchmark
{

public:
    static const std::function<void(void *)> task;
    template <typename T>
    void getTasks(T **args, int numberOfTasks) {}
    void workerTask(void *threadArgs) {}
};