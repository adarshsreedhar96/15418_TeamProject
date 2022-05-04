#include <functional>
typedef struct {
    const std::function<void(void*)> &task;
    void *args;
} Task;
