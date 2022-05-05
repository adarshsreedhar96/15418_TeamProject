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
    // why is it mutable though
    // mutable std::mutex queue_mutex = {};
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
            tasks.push(std::function<void()>(task));
        }
    }
    // push_task with arguments
    void push_task(const std::function<void(void *)> &task, void *args)
    {
        {
            tasksWithTaskStruct.push({task, args});
        }
    }
    bool pop_task(std::function<void()> &task)
    {
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
    bool isEmpty()
    {
        return tasks.empty();
    }
    int getSize(bool getSizeOfQueueWithTasks)
    {
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
        while (!tasks.empty())
        {
            tasks.pop();
        }
    }
};