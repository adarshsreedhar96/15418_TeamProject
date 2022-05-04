#include <stdio.h>
#include <thread>
#include <iostream>
#include "queue.h"
#include <atomic>
#include <stdlib.h>
#include <typeinfo>

using namespace std;
class threadPool_PerThread{
    private:

    public:
        uint32_t num_of_threads;
        std::atomic_bool runningFlag = ATOMIC_VAR_INIT(false);
        std::atomic_bool breakFlag = ATOMIC_VAR_INIT(false);
        std::atomic_bool isArgumentsPresent = ATOMIC_VAR_INIT(false);
        std::vector<std::thread> myThreads;
        std::vector<Queue> myQueues;
        // constructor
        threadPool_PerThread(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
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
                myThreads.push_back(std::thread(&threadPool_PerThread::worker, this, i));
            }
        }

        void worker(int index){
            //printf("worker() called with thread_id: %d\n", index);
            while(true){
                if(runningFlag){
                    if(isArgumentsPresent){
                        if(index==0){
                            //printf("came here: %d\n", myQueues[index].getSize(true));
                        }
                        // grab a task
                        std::function<void(void*)> newTask;
                        void* args;
                        if (myQueues[index].pop_task(newTask, &args))
                        {
                            if(index == 0){
                                //printf("run task: %d\n", index);
                            }
                            newTask(args);
                        } else {
                            // this means there are no entries in the queue.
                            // Attempt to steal tasks
                            if((!stealTasks(index)) && breakFlag){
                            //if(breakFlag){
                                break;
                            }
                            //printf("size of queue for thread: %d is %d\n", index, myQueues[index].getSize(true));
                        }
                    } else {
                        // grab a task
                        std::function<void()> task;
                        if (myQueues[index].pop_task(task))
                        {
                            task();
                        } else {
                            // this means there are no entries in the queue.
                            // Attempt to steal tasks
                            if((!stealTasks(index)) && breakFlag){
                            //if(breakFlag){
                                break;
                            }
                        }
                    }
                }
            }
        }
        bool stealTasks(int index){
            printf("thread %d is trying to steal\n");
            // iterate over the others in a ring manner
            for(int i=index+1;i<num_of_threads;i++){
                printf("thread: %d has queue of size: %d\n", i, myQueues[i].getSize(true));
                if(isArgumentsPresent){
                    // grab a task
                    std::function<void(void*)> newTask;
                    void* args;
                    //void** args;
                    if (myQueues[i].pop_task(newTask, &args)){
                        // we found a task! Lets steal it
                        printf("stealing task from thread: %d and giving to thread: %d\n", i, index);
                        newTask(args);
                        //myQueues[index].push_task(newTask, &args);

                        //myQueues[i%num_of_threads].push_task(task, *(args+i));
                        return true;
                    } else {
                        printf("queue of thread: %d is empty\n", index);
                    }
                } else {
                    std::function<void()> newTask;
                    if (myQueues[i].pop_task(newTask)){
                        // we found a task! Lets steal it
                        printf("stealing task from thread: %d and giving to thread: %d\n", i, index);
                        myQueues[index].push_task(newTask);
                        return true;
                    } else {
                        printf("queue of thread: %d is empty\n", index);
                    }
                }
            }
            return false;
        }
        void destroy_threads(){
            for (uint32_t i = 0; i < num_of_threads; i++)
            {
                myThreads[i].join();
            }
        }
        
        void submit(const std::function<void()> &task, int numberOfTasks){
            isArgumentsPresent = false;
            // put tasks onto each queue
            // but how do we access them, and also get track of their threads?
            for(int i=0;i<numberOfTasks;i++){
                myQueues[i%num_of_threads].push_task(task);
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
                myQueues[i%num_of_threads].push_task(task, *(args+i));
            }
        }
        void dispatch(){
            runningFlag = true;
        }
        void clearTasks(){
            breakFlag = true;
            destroy_threads();
        }
};