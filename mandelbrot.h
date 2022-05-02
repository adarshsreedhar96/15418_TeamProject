#include<stdio.h>

typedef struct {
    float x0, x1;
    float y0, y1;
    int width;
    int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
    // add the rows
    int startRow;
    int totalRows;
} WorkerArgs;


static inline int mandel(float c_re, float c_im, int count) {
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i) {
        if (z_re * z_re + z_im * z_im > 4.f)
            break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    return i;
}

void workerThreadStart(void *threadArgs) {

    WorkerArgs* args = static_cast<WorkerArgs *>(threadArgs);
    printf("workerThreadStart called by thread: %d\n", std::this_thread::get_id());
    // TODO: Implement worker thread here.
    float dx = (args->x1 - args->x0) / args->width;
    float dy = (args->y1 - args->y0) / args->height;

    int endRow = args->startRow + args->totalRows;

    for (int j = args->startRow; j < endRow; j++) {
        for (int i = 0; i < args->width; ++i) {
            float x = args->x0 + i * dx;
            float y = args->y0 + j * dy;

            int index = (j * args->width + i);
            args->output[index] = mandel(x, y, args->maxIterations);
        }
    }
}