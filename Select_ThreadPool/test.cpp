#include "threadpool.h"
#include <stdio.h>
void printEmtpy(){
    printf("helloworld called by thread: %d\n", std::this_thread::get_id());
}
int main(){
    threadPool threadPool(std::thread::hardware_concurrency());
    for(int i=0;i<8;i++){
        threadPool.submit(&printEmtpy);
    }
    return 0;
}