//#include "threadpool_centralized.h"
//#include "threadpool_perthread.h"
#include "threadpool_test.h"
#include <stdio.h>
void printEmtpy(){
    printf("helloworld called by thread: %d\n", std::this_thread::get_id());
}
int main(){
    #if 0
    threadPool threadPool(std::thread::hardware_concurrency());
    for(int i=0;i<8;i++){
        threadPool.submit(&printEmtpy);
    }
    #endif
    #if 1
    threadPool_test threadPool(std::thread::hardware_concurrency());
    threadPool.submit(&printEmtpy);
    #endif
    return 0;
}