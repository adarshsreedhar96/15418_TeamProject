#include<stdio.h>
#define VIEWCNT 7
/*
// basic initializations
const int width = 600;
// Height of the canvas
const int height = 600;
// Height of the actual image.  Make smaller to detect overrunning array
const int iheight = height - 1;
const int maxIterations = 256;
int numThreads = 2;

float x0 = -2.167;
float x1 = 1.167;
float y0 = -1;
float y1 = 1;

// Support VIEWCNT views
float scaleValues[VIEWCNT] = {0.01f, 1.0f,  0.015f, 0.02f, 0.02f, 0.02f, 0.002f};
float shiftXs[VIEWCNT] = {0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f, -1.4f};
float shiftYs[VIEWCNT] = {0.0f, 0.0f, 0.30f, 0.05f, 0.73f, 0.0f, 0.0f};
*/
void scaleAndShift(float &x0, float &x1, float &y0, float &y1, float scale,
                   float shiftX, float shiftY) {
    x0 *= scale;
    x1 *= scale;
    y0 *= scale;
    y1 *= scale;
    x0 += shiftX;
    x1 += shiftX;
    y0 += shiftY;
    y1 += shiftY;
}

bool verifyResult(int *gold, int *result, int width, int height) {
    int errLimit = 5;
    bool ok = true;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (gold[i * width + j] != result[i * width + j]) {
                printf("Mismatch : [%d][%d], Expected : %d, Actual : %d\n",
                       i, j, gold[i * width + j], result[i * width + j]);
                ok = false;
                if (--errLimit <= 0) {
                    printf(" ...\n");
                    return ok;
                }
            }
        }
    }

    return ok;
}

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

//mandelbrotserial
void mandelbrotSerial(float x0, float y0, float x1, float y1, int width,
                      int height, int startRow, int totalRows,
                      int maxIterations, int output[]) {
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    int endRow = startRow + totalRows;

    for (int j = startRow; j < endRow; j++) {
        for (int i = 0; i < width; ++i) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = (j * width + i);
            output[index] = mandel(x, y, maxIterations);
        }
    }
}

// workerThreadStart --
void *workerThreadStart(void *threadArgs) {
    WorkerArgs *args = static_cast<WorkerArgs *>(threadArgs);
    int endRow = args->startRow + args->totalRows;
    printf("startRow: %d to endRow:%d\n", args->startRow, endRow);
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
    return NULL;
}