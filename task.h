#include <functional>

enum StealAmount {
    STEALONETASK=0,
    STEALALLTASKS,
    STEALHALFTASKS 
} StealAmountType_t;

typedef struct {
    const std::function<void(void*)> &task;
    void *args;
} Task;
