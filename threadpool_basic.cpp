// This includes implementation of a simple centralized queue
// from threads running busy wait loops on the queue's state will
// grab work as and when it appears

// TODO: Fix Errors

#include<stdlib.h>
#include<thread>
#include<vector>
#include<mutex>
#include "threadpool_basic.h"
using namespace std;
class Thread_Pool{
public:
    void* Thread_Pool::busyWaitFunc(){
        while(true){
            {
                std::unique_lock<std::mutex> lock(lock);

            }
        }
        return NULL;
    }
    void Thread_Pool::pushToQueue(){
        std::unique_lock<std::mutex> lock(lock);
        // this is a private member, need to check how to access this
        Thread_Pool::queueOfTasks.push(func);
        // when we send the notification immediately, the consumer will try to get the lock , so unlock asap
        lock.unlock();
        // this needs to notify the conditions
        dataCondition.notify_one();
    }
    void Thread_Pool::popFromQueue(){
        std::unique_lock<std::mutex> lock(lock);
        lock.unlock();
        // we need to notify all threads
        dataCondition.notify_all();
    }

};
struct matrixArgs{
    int* matrixA;
    int* matrixB;
    int num_of_threads;
    int thread_id;
};

void* addition(void* threadArgs) {
    // typecast threadArgs, aince it is a void ptr
    matrixArgs* args = (matrixArgs*)threadArgs;
    // calculate linear index
    int start_index = (100/args->num_of_threads) * args->thread_id;
    int end_index   = std::min(((100/args->num_of_threads)) * (args->thread_id+1), 100);
    printf("hello world from thread: %d start_index: %d end_index: %d\n", args->thread_id, start_index, end_index);
    return NULL;
}
void createMatrixes(int* matrixA, int* matrixB){
    matrixA = (int*)malloc(sizeof(int) * 100);
    matrixB = (int*)malloc(sizeof(int) * 100);
}
int main() {
    // get the size of the concurrency
    int num_threads = thread::hardware_concurrency();
    // create a list of threads
    vector<std::thread> threads;
    for(int i=0;i<num_threads; i++){
        threads.push_back(thread(Thread_Pool::busyWaitFunc));
    }
    return 0;
}