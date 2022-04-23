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
            std::vector<Queue> myQueues(num_of_threads);
            //vector<queue<int>> myQueues(num_of_threads); // vector of queues
            //vec.push_back(queue<int>()); // add a queue 

            for(int i=0;i<num_of_threads;i++){
                //myQueues.push_back(queue<int>());
                myQueues.push_back(Queue());
            }
            
            // create the pool
            create_threads();
            // not sure if this is necessary
            // create_queues();
        }
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads[i] = thread(&threadPool_test::worker, this);
            }
        }
        void worker(){
            while(running){
                // grab a task
                std::function<void()> task;
                //if (myQueues.get(std::this_thread::get_id()-1).pop_task(task))
                //if(myQueues[i].get()->isEmpty())
                int thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
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
            for(int i=0;i<8;i++){
                //myQueues.get(i)->push_task(std::function<void()>(task));
                myQueues[i].push_task(std::function<void()>(task));
            }
            // now launch all of them
            running = true;
        }
        ~threadPool_test(){
            running = false;
            destroy_threads();
        }
};