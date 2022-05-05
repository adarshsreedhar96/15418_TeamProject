#include <stdio.h>
#include <thread>
#include "priorityq.h"
#include <atomic>
#include <stdlib.h>
#include <typeinfo>

using namespace std;
class threadPool_priority
{
private:
public:
    uint32_t num_of_threads;
    std::atomic_bool runningFlag = ATOMIC_VAR_INIT(false);
    std::atomic_bool breakFlag = ATOMIC_VAR_INIT(false);
    std::vector<std::thread> myThreads;
    bool *result;

    Queue queue;

    // constructor
    threadPool_priority(int numOfThreads, const std::function<bool(void *)> &taskFunc, bool *resultP)
    {
        printf("number of threads: %d\n", numOfThreads);
        num_of_threads = numOfThreads;
        result = resultP;
        // create an array of that many threads
        // myThreads = std::make_unique<std::thread[]>(num_of_threads);
        // create the pool
        create_threads(taskFunc);
    }
    void create_threads(const std::function<bool(void *)> &taskFunc)
    {
        for (int i = 0; i < num_of_threads; i++)
        {
            myThreads.push_back(thread(&threadPool_priority::worker, this, taskFunc));
        }
    }
    void worker(const std::function<bool(void *)> &taskFunc)
    {
        while (true)
        {
            if (runningFlag)
            {
                // grab a task
                std::function<bool(void *)> task;
                void *args;
                if (queue.pop_task(task, &args))
                {
                    if (taskFunc(args))
                    {
                        // drain queue
                        *result = true;
                        queue.drain_queue();
                        breakFlag = true;
                    }
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
    void submit(const std::function<bool(void *)> &task, T **args, int *priority, int numberOfTasks)
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
    void clearTasks()
    {
        breakFlag = true;
        destroy_threads();
    }
};