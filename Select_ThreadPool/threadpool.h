#include<stdio.h>
#include<thread>
#include "queue.h"
#include <atomic>
#include<stdlib.h>
#include <pthread.h>
using namespace std;
class threadPool{
    private:

    public:
        uint32_t num_of_threads;
        std::atomic<bool> running = true;
        std::thread *myThreads;
        //pthread_t *myThreads;
        // fields
        // methods
        // create a queue - for queue of tasks
        // Note that this requires 
        Queue queue;
        // constructor
        threadPool(int numOfThreads){
            printf("number of threads: %d\n", numOfThreads);
            num_of_threads = numOfThreads;
            // create an array of that many threads
            // think about this later - whether we need to delete it
            myThreads = (std::thread*)(malloc(sizeof(std::thread) * num_of_threads));
            //myThreads = (pthread_t*)(malloc(sizeof(pthread_t)*num_of_threads));
            // create the pool
            create_threads();
        }
        void create_threads(){
            for(int i=0;i<num_of_threads;i++){
                printf("creating thread id: %d\n", i);
                //myThreads[i] =  std::thread(&threadPool::worker, this);
                myThreads[i] = thread(&execute, this);
                printf("created thread id: %d\n", i);
                /*
                int err = pthread_create(&myThreads[i], NULL, &execute, NULL);
                if (err)
                    printf("created thread id: %d\n", i);
                else 
                printf("error while creating pthread]n");
                */
            }
        }
        void execute()
        {
            for(int i = 0; i < 1; i++)
            {
                //std::cout<<command<<" :: "<<i<<std::endl;
                printf("helloworld\n");
            }
        }
        void worker(){
            printf("worker thread\n");
            for(;;){}
            /*
            while(running){
                printf("worker thread\n");
                // keep checking if the queue has any entries left
                //if(!queue.isEmpty()){
                    // grab a task
                    std::function<void()> task;
                    if (queue.pop_task(task))
                    {
                        printf("thread got a task\n");
                        task();
                    }
                //}
            }*/
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
            printf("dtor\n");
            uint32_t sleep_duration = 1000;
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_duration));
         running = false;
            free(this->myThreads);
            destroy_threads();
        }
};