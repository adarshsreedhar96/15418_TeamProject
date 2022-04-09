#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional>
#include <mutex>
using namespace std;
/**
 * @brief A queue of tasks to be executed by the threads.
 * 
 */
class Queue{
    private:
        // fields
        // initialize an emtpy queue
        std::queue<function<void()>> tasks = {};
        /**
         * @brief A mutex to synchronize access to the task queue 
         * by different threads.
         */
        // why is it mutable though
        mutable std::mutex queue_mutex = {};
        // methods
    public:
        void push_task(const std::function<void()> &task)
        {
            //printf("entry added to queue\n");
            //tasks_total++;
            {
                const std::scoped_lock lock(queue_mutex);
                tasks.push(std::function<void()>(task));
            }
        }
        bool pop_task(std::function<void()> &task)
        {
            
            const std::scoped_lock lock(queue_mutex);
            if (tasks.empty()){
                //printf("not poppoing\n");
                return false;
            }
            else
            {
                //printf("poppoing\n");
                task = std::move(tasks.front());
                tasks.pop();
                return true;
            }
        }
        void print_tasks() {
            queue tmp_q = tasks; //copy the original queue to the temporary queue
            while (!tmp_q.empty())
            {
                std::function<void()> q_element = tmp_q.front();
                printf("%s\n", q_element);
                tmp_q.pop();
            }
        }
        bool isEmpty(){
            return tasks.empty(); 
        }

};