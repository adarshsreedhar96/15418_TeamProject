#include <stdio.h>
#include <thread>
#include "priorityq.h"
#include <atomic>
#include <stdlib.h>

using namespace std;
class threadPool_priority
{
private:
public:
    uint32_t num_of_threads;
    std::atomic<bool> runningFlag = {false};
    std::atomic<bool> breakFlag = {false};
    std::vector<std::thread> myThreads;
    std::function<void(void *)> &taskFunc;
    Queue queue;

    // constructor
    threadPool_priority(int numOfThreads, std::function<void(void *)> &task) : taskFunc(taskFunc)
    {
        taskFunc = task;
        printf("number of threads: %d\n", numOfThreads);
        num_of_threads = numOfThreads;
        // create an array of that many threads
        // myThreads = std::make_unique<std::thread[]>(num_of_threads);
        // create the pool
        create_threads();
    }
    void create_threads()
    {
        for (int i = 0; i < num_of_threads; i++)
        {
            myThreads.push_back(thread(&threadPool_priority::worker, this));
        }
    }
    void worker()
    {
        while (true)
        {
            if (runningFlag)
            {
                // grab a task
                std::function<void(void *)> task;
                void *args;
                if (queue.pop_task(task, &args))
                {
                    taskFunc(args);
                }
                else
                {
                    // this means there are no entries in the queue. We can exit the loop, but let us ensure that
                    // the user also wants to close down
                    if (breakFlag)
                    {
                        break;
                    }
                }
            }
        }
    }
    void destroy_threads()
    {
        for (uint32_t i = 0; i < num_of_threads; i++)
        {
            myThreads[i].join();
        }
    }
    // this allows adding an entry to the queue
    template <typename T>
    void submit(const std::function<void(void *)> &task, T **args, int *priority, int numberOfTasks)
    {
        // add tasks to queue?
        for (int i = 0; i < numberOfTasks; i++)
        {
            queue.push_task(task, *(args + i), priority[i]);
        }
    }
    // this ensures that initially all the tasks are queued up properly before consumption
    void dispatch()
    {
        runningFlag = true;
    }
    // this dtor waits for all threads to join
    ~threadPool_priority()
    {
        breakFlag = true;
        destroy_threads();
    }
};