#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional>
#include <mutex>
using namespace std;

#include <functional>
struct Task
{
    int priority;
    void *args;
};

/**
 * @brief A queue of tasks to be executed by the threads.
 *
 */
class Queue
{

private:
    // fields
    // initialize an emtpy queue
    std::vector<Task> tasks;
    /**
     * @brief A mutex to synchronize access to the task queue
     * by different threads.
     */
    // why is it mutable though
    std::mutex *queue_mutex;
    // methods
public:
    Queue()
    {
        // tasks = {};
        queue_mutex = new std::mutex();
    }

    // push_task with arguments
    void push_task(const std::function<bool(void *)> &task, void *args, int priority)
    {

        const std::lock_guard<std::mutex> lock(*queue_mutex);
        tasks.push_back({priority, args});
    }

    bool pop_task(std::function<bool(void *)> &task, void **args)
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        if (tasks.empty())
        {
            return false;
        }
        else
        {
            int maxPriority = -1;
            int idx = 0;
            for (int i = 0; i < tasks.size(); i++)
            {
                if (tasks[i].priority > maxPriority)
                {
                    maxPriority = tasks[i].priority;
                    idx = i;
                }
            }
            Task dequeuedTask = tasks.at(idx);
            // std::remove(tasks.begin(), tasks.end(), dequeuedTask);
            tasks.erase(tasks.begin() + idx);
            *args = dequeuedTask.args;
            return true;
        }
    }
    void drain_queue()
    {
        const std::lock_guard<std::mutex> lock(*queue_mutex);
        tasks.erase(tasks.begin(), tasks.end());
    }
    bool isEmpty()
    {
        return tasks.empty();
    }
};