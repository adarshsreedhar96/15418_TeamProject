// This includes the header file for a simple implementation
// of a centralized worked queue from which threads can
// steal tasks
#pragma once
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <queue>
#include <functional>

class Thread_Pool
{

private:
    std::atomic<bool> acceptFunctions;
    std::condition_variable dataCondition;
    std::queue<std::function<void()>> queueOfTasks;
    std::mutex lock;

public:

    Thread_Pool();
    ~Thread_Pool();
    void busyWaitFunc();
    void pushToQueue(std::function<void()> func);
    void popFromQueue();
};