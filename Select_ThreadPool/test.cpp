#include "threadpool.h"
#include <stdio.h>
void printEmtpy(){
    printf("helloworld\n");
}
int main(){
    threadPool threadPool(std::thread::hardware_concurrency());
    threadPool.submit(&printEmtpy);
    return 0;
}