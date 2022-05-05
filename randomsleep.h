typedef struct
{
    int taskNum;
    int numberOfThreads;
} TaskNumArgs;

void printTaskNum(void *task_num)
{
    TaskNumArgs *args = static_cast<TaskNumArgs *>(task_num);
    printf("helloworld called by task_num: %d\n", args->taskNum);
    srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    // int timeToSleep = rand() % 10;
    int timeToSleep = args->taskNum % args->numberOfThreads;
    // printf("thread_id: %d sleep for %d\n", std::this_thread::get_id(), timeToSleep);
    sleep(timeToSleep);
}

void printEmpty()
{
    // printf("helloworld called by thread: %d\n", std::this_thread::get_id());
    srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    int timeToSleep = rand() % 10;
    // printf("thread_id: %d sleep for %d\n", std::this_thread::get_id(), timeToSleep);
    sleep(timeToSleep);
}