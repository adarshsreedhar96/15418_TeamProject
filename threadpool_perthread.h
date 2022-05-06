/**
 * Here, we create a pool of threads, each of which gets its own queue for pushing tasks.
 * In the submit method, we then distribute the tasks in round-robin fashion across the threads. Future improvements can include
 * taking a custom function from the user to distribute the tasks.
 *
 * There are several parameters available: Run without Work Stealing, Run with Work Stealing (Different Types and Granularitites of Work to Steal).
 * Refer README.md for more details on how to change them or use them.
 */
#include <stdio.h>
#include <thread>
#include <iostream>

#include <atomic>
#include <stdlib.h>
#include <typeinfo>
#include <random>

#define STEALTASKS 1

#if STEALTASKS
#include "queue.h"
#else
#include "queue_nolock.h"
#endif

using namespace std;
class threadPool_PerThread
{
private:
public:
    uint32_t num_of_threads;
    std::atomic_bool runningFlag = ATOMIC_VAR_INIT(false);
    std::atomic_bool breakFlag = ATOMIC_VAR_INIT(false);
    std::atomic_bool isArgumentsPresent = ATOMIC_VAR_INIT(false);
    std::vector<std::thread> myThreads;
    std::vector<Queue> myQueues;
    bool toSteal = false;
    StealAmount stealAmount;
    StealType stealType;
    int stealCount = 0;
    // constructor
    threadPool_PerThread(int numOfThreads, bool toSteal, StealAmount stealAmount, StealType stealType)
    {
        this->num_of_threads = numOfThreads;
        this->toSteal = toSteal;
        this->stealAmount = stealAmount;
        this->stealType = stealType;

        // create an array of that many threads
        for (int i = 0; i < num_of_threads; i++)
        {
            // create a new queue instance
            myQueues.push_back(Queue());
        }
        // create the pool
        create_threads();
        // not sure if this is necessary
    }
    void create_threads()
    {
        for (int i = 0; i < num_of_threads; i++)
        {
            myThreads.push_back(std::thread(&threadPool_PerThread::worker, this, i));
        }
    }
    void worker(int index)
    {
        while (true)
        {
            if (runningFlag)
            {
                if (isArgumentsPresent)
                {
                    // grab a task
                    std::function<void(void *)> newTask;
                    void *args;
                    if (myQueues[index].pop_task(newTask, &args))
                    {
                        newTask(args);
                    }
                    else
                    {
                        // this means there are no entries in the queue.
                        // Attempt to steal tasks
                        if (toSteal && (!stealTasks(index)) && breakFlag)
                        {
                            break;
                        }
                        else if (breakFlag)
                        {
                            break;
                        }
                        // printf("size of queue for thread: %d is %d\n", index, myQueues[index].getSize(true));
                    }
                }
                else
                {
                    // grab a task
                    std::function<void()> task;
                    if (myQueues[index].pop_task(task))
                    {
                        task();
                    }
                    else
                    {
                        // this means there are no entries in the queue.
                        // Attempt to steal tasks
                        if (toSteal && (!stealTasks(index)) && breakFlag)
                        {
                            break;
                        }
                        else if (breakFlag)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    bool stealTasks(int index)
    {
        if (this->stealType == STEALNEIGHBOURTASK)
        {
            // iterate over the others in a ring manner
            for (int i = index + 1; i < num_of_threads; i++)
            {
                // printf("thread: %d has queue of size: %d\n", i, myQueues[i].getSize(true));
                if (isArgumentsPresent)
                {
                    // create an array of Tasks
                    std::vector<Task> stolenTasks;
                    // grab a task
                    if (myQueues[i].steal_task(&stolenTasks, stealAmount))
                    {
                        // we found a task! Lets steal it
                        while (!stolenTasks.empty())
                        {
                            stealCount++;
                            Task poppedTask = stolenTasks.back();
                            std::function<void(void *)> newTask = poppedTask.task;
                            void *args = poppedTask.args;
                            newTask(args);
                            stolenTasks.pop_back();
                        }
                        return false;
                    }
                    else
                    {
                        // printf("queue of thread: %d is empty\n", index);
                    }
                }
                else
                {
                    std::function<void()> newTask;
                    if (myQueues[i].pop_task(newTask))
                    {
                        // we found a task! Lets steal it
                        // printf("stealing task from thread: %d and giving to thread: %d\n", i, index);
                        myQueues[index].push_task(newTask);
                        return true;
                    }
                    else
                    {
                        // printf("queue of thread: %d is empty\n", index);
                    }
                }
            }
            return false;
        }
        else if (this->stealType == STEALRANDOMTASK)
        {
            std::random_device device;
            std::mt19937 randomNumberGenerator(device());
            std::uniform_int_distribution<std::mt19937::result_type> distCurrentSize(0, this->num_of_threads - 1);
            int randomlyChosenIndex = distCurrentSize(randomNumberGenerator);
            // make sure that when we randomly choose an index, it is not the same index
            while (randomlyChosenIndex == index)
            {
                randomlyChosenIndex = distCurrentSize(randomNumberGenerator);
            }
            // printf("rsndomly stealing from thread%d\n",randomlyChosenIndex);
            if (isArgumentsPresent)
            {
                // create an array of Tasks
                std::vector<Task> stolenTasks;
                // grab a task
                void *args;
                if (myQueues[randomlyChosenIndex].steal_task(&stolenTasks, stealAmount))
                {
                    while (!stolenTasks.empty())
                    {
                        stealCount++;
                        Task poppedTask = stolenTasks.back();
                        std::function<void(void *)> newTask = poppedTask.task;
                        void *args = poppedTask.args;
                        newTask(args);
                        stolenTasks.pop_back();
                    }
                    return false;
                }
                else
                {
                    // the queue that we randomly tried to steal from was already empty!
                    // exit for now, but as a future scope, we can keep attempting to steal until we get a task
                    return false;
                }
            }
            else
            {
                std::function<void()> newTask;
                if (myQueues[randomlyChosenIndex].pop_task(newTask))
                {
                    // we found a task! Lets steal it
                    myQueues[randomlyChosenIndex].push_task(newTask);
                    return true;
                }
                else
                {
                    // the queue that we randomly tried to steal from was already empty!
                    // exit for now, but as a future scope, we can keep attempting to steal until we get a task
                    return false;
                }
            }
        }
        else
        {
            printf("invalid steal type given, skipping\n");
            return false;
        }
    }
    void destroy_threads()
    {
        for (uint32_t i = 0; i < num_of_threads; i++)
        {
            myThreads[i].join();
        }
    }
    void submit(const std::function<void()> &task, int numberOfTasks)
    {
        isArgumentsPresent = false;
        // put tasks onto each queue
        // but how do we access them, and also get track of their threads?
        for (int i = 0; i < numberOfTasks; i++)
        {
            myQueues[i % num_of_threads].push_task(task);
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
            myQueues[i % num_of_threads].push_task(task, *(args + i));
        }
    }
    void dispatch()
    {
        runningFlag = true;
    }
    void clearTasks()
    {
        breakFlag = true;
        destroy_threads();
        printf("Number of stolen tasks is %d\n", stealCount);
    }
};