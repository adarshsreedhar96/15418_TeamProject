#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional>
#include <mutex>
#include "task.h"

using namespace std;

/**
 * @brief A queue of tasks to be executed by the threads.
 *
 */
class Queue
{

private:
    // fields
    // initialize an emtpy queue
    std::queue<function<void()>> tasks;
    std::queue<Task> tasksWithTaskStruct;
    /**
     * @brief A mutex to synchronize access to the task queue
     * by different threads.
     */
    std::mutex *queue_mutex;
    // methods
public:
    Queue()
    {
        tasks = {};
        tasksWithTaskStruct = {};
        queue_mutex = new std::mutex();
    }
    // push_task with no arguments
    void push_task(const std::function<void()> &task)
    {
        {
            // const std::scoped_lock lock(queue_mutex);
            const std::lock_guard<std::mutex> lock(*queue_mutex);
            tasks.push(std::function<void()>(task));
        }
    }
    // push_task with arguments
    void push_task(const std::function<void(void *)> &task, void *args)
    {
        {
            // const std::scoped_lock lock(queue_mutex);
            const std::lock_guard<std::mutex> lock(*queue_mutex);
            tasksWithTaskStruct.push({task, args});
        }
    }
    bool pop_task(std::function<void()> &task)
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        if (tasks.empty())
        {
            return false;
        }
        else
        {
            task = std::move(tasks.front());
            tasks.pop();
            return true;
        }
    }
    bool pop_task(std::function<void(void *)> &task, void **args)
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        if (tasksWithTaskStruct.empty())
        {
            return false;
        }
        else
        {
            Task dequeuedTask = std::move(tasksWithTaskStruct.front());
            tasksWithTaskStruct.pop();
            task = dequeuedTask.task;
            *args = dequeuedTask.args;
            return true;
        }
    }
    bool steal_task(std::vector<Task> *stolenTasks, StealAmount amount)
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        if (tasksWithTaskStruct.empty())
        {
            return false;
        }
        else
        {
            // steal half the tasks
            int currentSize = tasksWithTaskStruct.size();
            int tasksToSteal = 1; // default number of tasks to steal
            if (amount == STEALALLTASKS)
            {
                tasksToSteal = currentSize;
            }
            else if (amount == STEALHALFTASKS)
            {
                tasksToSteal = currentSize / 2;
            }
            else
            {
                tasksToSteal = 1;
            }
            for (int i = 0; i < tasksToSteal; i++)
            {
                Task dequeuedTask = std::move(tasksWithTaskStruct.front());
                tasksWithTaskStruct.pop();
                stolenTasks->push_back(dequeuedTask);
            }
            // task = dequeuedTask.task;
            //*args = dequeuedTask.args;
            return true;
        }
    }
    bool isEmpty()
    {
        return tasks.empty();
    }
    int getSize(bool getSizeOfQueueWithTasks)
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        if (getSizeOfQueueWithTasks)
        {
            return tasksWithTaskStruct.size();
        }
        else
        {
            return tasks.size();
        }
    }
    void drain_queue()
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        while (!tasksWithTaskStruct.empty())
        {
            tasksWithTaskStruct.pop();
        }
    }
};