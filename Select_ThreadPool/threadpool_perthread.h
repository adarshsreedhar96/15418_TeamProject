#include<stdio.h>
#include<thread>
#include "queue.h"
#include <atomic>
#include<stdlib.h>

using namespace std;
class threadPool_PerThread{
    private:

    public:
        uint32_t num_of_threads;
        std::atomic<bool> running = false;
        std::unique_ptr<thread[]> myThreads;
        //std::unique_ptr<Queue[]> myQueues;
        std::vector<Queue> myQueues;
        // constructor
        threadPool_PerThread(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
            myThreads = std::make_unique<thread[]>(num_of_threads);
            //myQueues  = std::make_unique<Queue[]>(num_of_threads);
            for(int i=0;i<num_of_threads; i++){
                Queue queue;
                myQueues.push_back(queue); 
            }
            /*
            vector<Queue> myQueues;
            for(int i=0;i<num_of_threads; i++){
                Queue queue;
                this->myQueues[i]=queue;
            }
            */
            
            // instantiate 
            // create the pool
            create_threads();
        }
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads[i] = thread(&threadPool_PerThread::worker, this);
            }
        }
        Queue getQueueHandle(int thread_id){
            return myQueues[thread_id-1];
        }
        void worker(){
            // should we have a lock to access the array?
            //Queue tasks = getQueueHandle(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            auto tasks = myQueues.at(std::hash<std::thread::id>{}(std::this_thread::get_id())-1);
            //Queue tasks = myQueues[std::hash<>];
            while(running){
                // grab a task
                std::function<void()> task;
                if (tasks.pop_task(task))
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
            // put tasks onto each queue
            // but how do we access them, and also get track of their threads?
            for(int i=0;i<8;i++){
                myQueues[i].push_task(std::function<void()>(task));
            }
            // now launch all of them
            running = true;
        }
        ~threadPool_PerThread(){
            running = false;
            destroy_threads();
        }
};