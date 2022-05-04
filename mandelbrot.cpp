#include "benchmark.h"
#include <string.h>
#include "mandelbrot.h"

using namespace std;
class Mandelbrot : Benchmark
{
    private:
    /*
        // basic initializations
        const int width = 600;
        // Height of the canvas
        const int height = 600;
        // Height of the actual image.  Make smaller to detect overrunning array
        const int iheight = height - 1;
        const int maxIterations = 256;
        //const int numOfThreads = std::thread::hardware_concurrency();

        float x0 = -2.167;
        float x1 = 1.167;
        float y0 = -1;
        float y1 = 1;

        // Support VIEWCNT views
        float scaleValues[VIEWCNT] = {0.01f, 1.0f,  0.015f, 0.02f, 0.02f, 0.02f, 0.002f};
        float shiftXs[VIEWCNT] = {0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f, -1.4f};
        float shiftYs[VIEWCNT] = {0.0f, 0.0f, 0.30f, 0.05f, 0.73f, 0.0f, 0.0f};

        int viewIndex = 6;
        // // End parsing of commandline options
        float scaleValue = scaleValues[viewIndex];
        float shiftX = shiftXs[viewIndex];
        float shiftY = shiftYs[viewIndex];

        int *output_serial = new int[width * height];
        int *output_thread = new int[width * height];
        */
    public:
    // Threading support
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

    void setInput(int viewIndex, int numOfThreads)
    {   
        numOfThreads = numOfThreads;
        viewIndex = viewIndex;
        memset(output_thread, 0, width * height * sizeof(int));
    }

    static void workerTask(void *threadArgs)
    {
        printf("workerThreadStart called by thread: %d\n", std::this_thread::get_id());
        WorkerArgs *args = static_cast<WorkerArgs *>(threadArgs);
        int endRow = args->startRow + args->totalRows;
        //printf("startRow: %d endRow: %d\n", args->startRow, endRow);
        float dx = (args->x1 - args->x0) / args->width;
        float dy = (args->y1 - args->y0) / args->height;
        //for (int j = args->startRow; j < args->totalRows; j+=args->numThreads) {
        for (int j = args->startRow; j < endRow; j++) {
            for (int i = 0; i < args->width; ++i) {
                float x = args->x0 + i * dx;
                float y = args->y0 + j * dy;
                int index = (j * args->width + i);
                args->output[index] = mandel(x, y, args->maxIterations);
            }
        }
        //return NULL
    }
    template <typename T>
    void getTasks(T **args, int numberOfTasks)
    {
        //int *output_thread = new int[width * height];
        for (int i = 0; i < numberOfTasks; i++) {
            args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
            args[i]->threadId = i;
            args[i]->x0 = x0;
            args[i]->y0 = y0;
            args[i]->x1 = x1;
            args[i]->y1 = y1;
            args[i]->height = height;
            args[i]->width  = width;
            args[i]->maxIterations = maxIterations;
            args[i]->output = output_thread;
            args[i]->numThreads= numOfThreads;
            args[i]->totalRows = height/numberOfTasks;
            args[i]->startRow  = i*args[i]->totalRows;
            if((i+1)==numberOfTasks){
                args[i]->totalRows  = height-args[i]->startRow;
            }
        }
    }
};