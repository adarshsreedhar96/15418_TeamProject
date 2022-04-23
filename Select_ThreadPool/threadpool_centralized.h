#include<stdio.h>
#include<thread>
#include "queue.h"
#include <atomic>
#include<stdlib.h>

using namespace std;
class threadPool{
    private:

    public:
        uint32_t num_of_threads;
        std::atomic<bool> running = true;
        std::unique_ptr<std::thread[]> myThreads;
        Queue queue;
        // constructor
        threadPool(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
            myThreads = std::make_unique<std::thread[]>(num_of_threads);
            // create the pool
            create_threads();
        }
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads[i] = thread(&threadPool::worker, this);
            }
        }
        void worker(){
            while(running){
                // grab a task
                std::function<void()> task;
                if (queue.pop_task(task))
                {
                    task();
                }
            }
        }
        void destroy_threads(){
            for (uint32_t i = 0; i < num_of_threads; i++)
            {
                myThreads[i].join();
            }
        }
        void submit(const std::function<void()> &task){
            // add tasks to queue?
            queue.push_task(task);
        }
        ~threadPool(){
            running = false;
            destroy_threads();
        }
};