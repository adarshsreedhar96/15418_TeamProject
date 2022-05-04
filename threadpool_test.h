#include<stdio.h>
#include<thread>
#include "queue.h"
#include <atomic>
#include<stdlib.h>

using namespace std;
class threadPool_test{
    private:

    public:
        uint32_t num_of_threads;
        std::atomic<bool> running = true;
        std::unique_ptr<std::thread[]> myThreads;
        //std::unique_ptr<Queue> myQueues;
        std::vector<Queue> myQueues;
        //Queue queue;
        // constructor
        threadPool_test(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
            myThreads = std::make_unique<std::thread[]>(num_of_threads);
            //myQueues = std::make_unique<Queue>(num_of_threads);
           // std::vector<Queue> myQueues(num_of_threads);
            //vector<queue<int>> myQueues(num_of_threads); // vector of queues
            //vec.push_back(queue<int>()); // add a queue 

            for(int i=0;i<num_of_threads;i++){
                //myQueues.push_back(queue<int>());
                myQueues.push_back(Queue());
            }
            printf("queues length: %d\n", myQueues.size());
            // create the pool
            create_threads();
            // not sure if this is necessary
            // create_queues();
        }
        
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads[i] = thread(&worker, this, i);
            }
        }
        void worker(int thread_id){
            //int thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
            //uint64_t thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
             //printf("thread_id: %d\n", thread_id);
            while(running){
                // grab a task
               
                std::function<void()> task;
                //if (myQueues.get(std::this_thread::get_id()-1).pop_task(task))
                //if(myQueues[i].get()->isEmpty())
                if (myQueues[thread_id].pop_task(task))
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
            for(int i=0;i<num_of_threads;i++){
                //myQueues.get(i)->push_task(std::function<void()>(task));
                myQueues[i].push_task(std::function<void()>(task));
            }
            // now launch all of them
            //running = true;
        }
        // auto maybe a struct, need to check when to use its fields
        void submit(const std::function<void()> &task, auto params){
            // put tasks onto each queue
            // but how do we access them, and also get track of their threads?
            for(int i=0;i<num_of_threads;i++){
                //myQueues[i].push_task(std::function<void()>(task));
                myQueues[i].push_task(std::function<void()>(task), params);
            }
            // now launch all of them
            //running = true;
        }
        ~threadPool_test(){
            running = false;
            destroy_threads();
        }
};