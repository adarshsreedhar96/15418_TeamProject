#include "benchmark.h"
#include <string.h>
#include "randomsleep.h"

using namespace std;
class RandomSleep : Benchmark
{
    private:
    public:
    // Threading support
    int numberOfTasks;
    int numOfThreads;

    void setInput(int numberOfTasks, int numOfThreads)
    {   
        this->numberOfTasks = numberOfTasks;
        this->numOfThreads = numOfThreads;
    }

    static void workerTask(void *threadArgs)
    {
        //printf("randomSleep called by thread: %d\n", std::this_thread::get_id());
        TaskNumArgs *args = static_cast<TaskNumArgs *>(threadArgs);
        //printf("helloworld called by task_num: %d\n", args->taskNum);
        srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        //int timeToSleep = rand() % 10;
        int timeToSleep = args->taskNum % args->numberOfThreads;
        //printf("thread_id: %d sleep for %d\n", std::this_thread::get_id(), timeToSleep);
        sleep(timeToSleep);
        //return NULL
    }
    template <typename T>
    void getTasks(T **args, int numberOfTasks)
    {
        for (int i = 0; i < numberOfTasks; i++) {
            args[i] =  (TaskNumArgs*) malloc(sizeof(TaskNumArgs));
            args[i]->taskNum = i;
            args[i]->numberOfThreads = this->numOfThreads;
        }
    }
};