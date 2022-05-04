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
    void push_task(const std::function<void(void *)> &task, void *args, int priority)
    {

        const std::lock_guard<std::mutex> lock(*queue_mutex);
        tasks.push_back({priority, args});
    }

    bool pop_task(std::function<void(void *)> &task, void **args)
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
                printf("elemnt idx %d has start idx %d\n", i, tasks[i].priority);
                if (tasks[i].priority > maxPriority)
                {
                    maxPriority = tasks[i].priority;
                    idx = i;
                }
            }
            printf("idx is %d\n", idx);
            Task dequeuedTask = tasks.at(idx);
            // std::remove(tasks.begin(), tasks.end(), dequeuedTask);
            printf("elemnt begin has priority %d\n", tasks.begin()->priority);
            tasks.erase(tasks.begin() + idx);
            printf("0th task of queue priority %d\n", tasks[0].priority);
            *args = dequeuedTask.args;
            return true;
        }
    }
    // void print_tasks()
    // {
    //     std::queue<function<void()>> tmp_q = tasks; // copy the original queue to the temporary queue
    //     while (!tmp_q.empty())
    //     {
    //         std::function<void()> q_element = tmp_q.top();
    //         // printf("%s\n", q_element);
    //         tmp_q.pop();
    //     }
    // }
    bool isEmpty()
    {
        return tasks.empty();
    }
};