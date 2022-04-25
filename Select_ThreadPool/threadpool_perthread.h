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
        std::atomic<bool> runningFlag = false;
        std::atomic<bool> breakFlag = false;
        std::unique_ptr<std::thread[]> myThreads;
        std::vector<Queue> myQueues;
        // constructor
        threadPool_PerThread(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
            myThreads = std::make_unique<std::thread[]>(num_of_threads);
            for(int i=0;i<num_of_threads;i++){
                // create a new queue instance
                myQueues.push_back(Queue());
            }
            printf("queues length: %d\n", myQueues.size());
            // create the pool
            create_threads();
            // not sure if this is necessary
        }  
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads[i] = thread(&worker, this, i);
            }
        }
        void worker(int index){
            while(true){
                if(runningFlag){
                    // grab a task
                    std::function<void()> task;
                    if (myQueues[index].pop_task(task))
                    {
                        task();
                    } else {
                        // this means there are no entries in the queue. We can exit the loop, but let us ensure that 
                        // the user also wants to close down
                        if(breakFlag){
                            break;
                        }
                    }
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
            //printf("myQueues size: %d\n", myQueues.size());
            // put tasks onto each queue
            // but how do we access them, and also get track of their threads?
            for(int i=0;i<num_of_threads;i++){
                myQueues[i].push_task(task);
            }
        }
        
        void dispatch(){
            runningFlag = true;
        }
        // dtor 
        ~threadPool_PerThread(){
            breakFlag = true;
            destroy_threads();
        }
};