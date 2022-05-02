#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional>
#include <mutex>
#include "task.h"
using namespace std;

// typedef struct {
//     const std::function<void(void*)> &task;
//     void *args;
// } Task;

/**
 * @brief A queue of tasks to be executed by the threads.
 *
 */
class Queue
{

private:
    // fields
    // initialize an emtpy queue
    std::queue<function<void()> > tasks;
    std::queue<Task> tasksWithTaskStruct;
    /**
     * @brief A mutex to synchronize access to the task queue
     * by different threads.
     */
    // why is it mutable though
    // mutable std::mutex queue_mutex = {};
    // methods
public:
    Queue()
    {
        tasks = {};
        tasksWithTaskStruct = {};
    }
    // push_task with no arguments
    void push_task(const std::function<void()> &task)
    {
        {
            // const std::scoped_lock lock(queue_mutex);
            tasks.push(std::function<void()>(task));
        }
    }

    // push_task with arguments
    void push_task(const std::function<void(void *)> &task, void *args)
    {
        {
            // const std::scoped_lock lock(queue_mutex);
            tasksWithTaskStruct.push({task, args});
        }
    }

    bool pop_task(std::function<void()> &task)
    {
        // const std::scoped_lock lock(queue_mutex);
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
    void print_tasks()
    {
        queue tmp_q = tasks; // copy the original queue to the temporary queue
        while (!tmp_q.empty())
        {
            std::function<void()> q_element = tmp_q.front();
            printf("%s\n", q_element);
            tmp_q.pop();
        }
    }
    bool isEmpty()
    {
        return tasks.empty();
    }
};