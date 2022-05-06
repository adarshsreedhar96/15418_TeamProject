/**
 * This implements a single queue where all tasks are put, and creates a threadpool all of whcih attempt to
 * grab tasks from the same queue. We introduce locks to avoid contention.
 * The way the threads work is that at creation, they run a busy-wait loop. After we put all the tasks in the
 * queue, we then set a flag, runningFlag to true (dispatch function), after which the threads begin to pop tasks from the queue.
 * Once threads find no more tasks to pop, they then check if they need to exit the loop. This option is given so that
 * the user can add more tasks necessary at runtime. Once the breakFlag is set to true (in the clearTasks function),
 * the threads slowly exit their work() methods, and since join() is called, eventually terminate.
 */
#include <stdio.h>
#include <thread>
#include "queue.h"
#include <atomic>
#include <stdlib.h>

using namespace std;
class threadPool
{
private:
public:
    // set in ctor
    uint32_t num_of_threads;
    // whether the threads running worker() need to start popping tasks
    std::atomic_bool runningFlag = ATOMIC_VAR_INIT(false);
    // whether the threads running worker() need to exit the loop
    std::atomic_bool breakFlag = ATOMIC_VAR_INIT(false);
    // whether the threads running
    std::atomic_bool isArgumentsPresent = ATOMIC_VAR_INIT(false);
    // stores all thread objects.
    std::vector<std::thread> myThreads;
    // creates the queue type.
    Queue queue;
    // constructor
    threadPool(int numOfThreads)
    {
        printf("number of threads: %d\n", numOfThreads);
        num_of_threads = numOfThreads;
        // create the pool
        create_threads();
    }
    void create_threads()
    {
        for (int i = 0; i < num_of_threads; i++)
        {
            myThreads.push_back(std::thread(&threadPool::worker, this, i));
        }
    }
    // main logic of each thread, runs this after its spawned.
    // this is a busy-wait loop, and it enters depending on value of runningFlag
    // the index parameter is the thread_id
    // isArguments present is a flag to check whether the tasks to be exeucted require
    // arguments or not
    // as mentioned above, each thread attempts to pop tasks and then run them
    // once queue returns empty, then we break the loop and exit function, depending on value of breakFlag
    void worker(int index)
    {
        printf("worker() called with thread_id: %d\n", index);
        while (true)
        {
            if (runningFlag)
            {
                if (isArgumentsPresent)
                {
                    // grab a task
                    std::function<void(void *)> newTask;
                    void *args;
                    if (queue.pop_task(newTask, &args))
                    {
                        newTask(args);
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
                else
                {
                    // grab a task
                    std::function<void()> task;
                    if (queue.pop_task(task))
                    {
                        task();
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
    }

    void destroy_threads()
    {
        for (uint32_t i = 0; i < num_of_threads; i++)
        {
            myThreads[i].join();
        }
    }
    // this allows adding an entry to the queue
    void submit(const std::function<void()> &task, int numberOfTasks)
    {
        isArgumentsPresent = false;
        // put tasks onto each queue
        // but how do we access them, and also get track of their threads?
        for (int i = 0; i < numberOfTasks; i++)
        {
            queue.push_task(task);
        }
    }

    template <typename T>
    void submit(const std::function<void(void *)> &task, T **args, int numberOfTasks)
    {
        isArgumentsPresent = true;
        // how do we get to know the size?
        // we split into num_of_threads for now
        for (int i = 0; i < numberOfTasks; i++)
        {
            // since number of threads and number of tasks may not be the same
            // need this
            queue.push_task(task, *(args + i));
        }
    }
    // this ensures that initially all the tasks are queued up properly before consumption
    void dispatch()
    {
        runningFlag = true;
    }
    // this clear waits for all threads to join
    void clearTasks()
    {
        breakFlag = true;
        destroy_threads();
    }
};