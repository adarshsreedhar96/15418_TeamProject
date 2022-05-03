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
        std::atomic_bool runningFlag = ATOMIC_VAR_INIT(false);
        std::atomic_bool breakFlag   = ATOMIC_VAR_INIT(false);
        std::atomic_bool isArgumentsPresent = ATOMIC_VAR_INIT(false);
        std::vector<std::thread> myThreads;
        Queue queue;
        // constructor
        threadPool(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create the pool
            create_threads();
        }
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                myThreads.push_back(std::thread(&threadPool::worker, this, i));
            }
        }
        void worker(int index){
            printf("worker() called with thread_id: %d\n", index);
            while(true){
                if(runningFlag){
                    if(isArgumentsPresent){
                        // grab a task
                        std::function<void(void*)> newTask;
                        void* args;
                        if (queue.pop_task(newTask, &args))
                        {
                            newTask(args);
                        } else {
                            // this means there are no entries in the queue. We can exit the loop, but let us ensure that 
                            // the user also wants to close down
                            if(breakFlag){
                                break;
                            }
                        }
                    } else {
                        // grab a task
                        std::function<void()> task;
                        if (queue.pop_task(task))
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
        }

        // void worker(){
        //     while(true){
        //         if(runningFlag){
        //             // grab a task
        //             std::function<void()> task;
        //             if (queue.pop_task(task))
        //             {
        //                 task();
        //             } else {
        //                 // this means there are no entries in the queue. We can exit the loop, but let us ensure that 
        //                 // the user also wants to close down
        //                 if(breakFlag){
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }

        void destroy_threads(){
            for (uint32_t i = 0; i < num_of_threads; i++)
            {
                myThreads[i].join();
            }
        }
        // this allows adding an entry to the queue
        void submit(const std::function<void()> &task, int numberOfTasks){
            isArgumentsPresent = false;
            // put tasks onto each queue
            // but how do we access them, and also get track of their threads?
            for(int i=0;i<num_of_threads;i++){
                queue.push_task(task);
            }
        }
        
        template <typename T>
        void submit(const std::function<void(void*)> &task, T** args, int numberOfTasks){
            isArgumentsPresent = true;
            // how do we get to know the size?
            // we split into num_of_threads for now
            for(int i=0;i<numberOfTasks;i++) {
                // since number of threads and number of tasks may not be the same
                // need this
                queue.push_task(task, *(args+i));
            }
        }
        // this ensures that initially all the tasks are queued up properly before consumption
        void dispatch(){
            runningFlag = true;
        }
        // this dtor waits for all threads to join
        void clearTasks(){
            breakFlag = true;
            destroy_threads();
        }
};