#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <strings.h>
#include "threadpool_perthread.h"
#include <stdlib.h>
#include "ppm.cpp"
// #include "CycleTimer.h"

// extern void mandelbrotSerial(float x0, float y0, float x1, float y1, int width,
//                              int height, int startRow, int numRows,
//                              int maxIterations, int output[]);

// extern void mandelbrotThread(int numThreads, float x0, float y0, float x1,
//                              float y1, int width, int height, int maxIterations,
//                              int output[]);

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
  int start_row;
  int num_rows;
  double thd_exec_time;
} WorkerArgs;

// extern void writePPMImage(int *data, int width, int height,
//                           const char *filename, int maxIterations);

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

void usage(const char *progname) {
  printf("Usage: %s [options]\n", progname);
  printf("Program Options:\n");
  printf("  -t  --threads <N>       Use N threads\n");
  printf("  -v  --view <INT>        Use specified view settings (0-6)\n");
  printf("  -f  --field x0:y0:x1:y1 Specify set boundaries\n");
  printf("  -o  outfile             Specify output file\n");
  printf("  -?  --help              This message\n");
}

bool verifyResult(int *gold, int *result, int width, int height) {
  int i, j;
  int errLimit = 5;
  bool ok = true;

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      if (gold[i * width + j] != result[i * width + j]) {
        printf("Mismatch : [%d][%d], Expected : %d, Actual : %d\n", i, j,
               gold[i * width + j], result[i * width + j]);
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

#define VIEWCNT 7

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

//
// MandelbrotSerial --
//
// Compute an image visualizing the mandelbrot set.  The resulting
// array contains the number of iterations required before the complex
// number corresponding to a pixel could be rejected from the set.
//
// * x0, y0, x1, y1 describe the complex coordinates mapping
//   into the image viewport.
// * width, height describe the size of the output image
// * startRow, totalRows describe how much of the image to compute
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


void *workerThreadStart(void *threadArgs) {

  float dx, dy;
  int j, end_row, stride;
  WorkerArgs *args = static_cast<WorkerArgs *>(threadArgs);
  printf("workerThreadStart called by thread: %d\n", std::this_thread::get_id());
  /* Do whatever "mandelbrotSerial()" does. */
  dx = (args->x1 - args->x0) / args->width;
  dy = (args->y1 - args->y0) / args->height;
  end_row = args->start_row + args->num_rows;
  end_row = std::min(end_row, 600);
  stride = 1;

  printf("threadStart: before startof loop\n");
  //if(std::hash<std::thread::id>{}(std::this_thread::get_id()) == 8){
  printf("start_row: %d thread_id: %d\n", args->start_row, std::this_thread::get_id());
  printf("end_row: %d thread_id: %d\n", end_row, std::this_thread::get_id());
  //}
  for (j = args->start_row; j < end_row; j += stride) {
    for (int i = 0; i < args->width; ++i) {
      float x = args->x0 + i * dx;
      float y = args->y0 + j * dy;

      int index = (j * args->width + i);
      if(std::hash<std::thread::id>{}(std::this_thread::get_id()) == 8){
        printf("index: %d\n", index);
      }
      args->output[index] = mandel(x, y, args->maxIterations);
    }
  }
 printf("workerThreadStart end called by thread: %d\n", std::this_thread::get_id());
  return NULL;
}


int main(int argc, char **argv) {
  const int width = 600;
  // Height of the canvas
  const int height = 600;
  // Height of the actual image.  Make smaller to detect overrunning array
  const int iheight = height - 1;
  const int maxIterations = 256;
  int numThreads = 8;

  float x0 = -2.167;
  float x1 = 1.167;
  float y0 = -1;
  float y1 = 1;

  // Support VIEWCNT views
  float scaleValues[VIEWCNT] = {0.01f, 1.0f,  0.015f, 0.02f,
                                0.02f, 0.02f, 0.002f};
  float shiftXs[VIEWCNT] = {0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f, -1.4f};
  float shiftYs[VIEWCNT] = {0.0f, 0.0f, 0.30f, 0.05f, 0.73f, 0.0f, 0.0f};

  // parse commandline options ////////////////////////////////////////////
  int opt;
  static struct option long_options[] = {
      {"threads", 1, 0, 't'}, {"view", 1, 0, 'v'}, {"field", 1, 0, 'f'},
      {"out", 1, 0, 'o'},     {"help", 0, 0, '?'}, {0, 0, 0, 0}};

  int viewIndex = 0;
  char fname[256];
  bool have_file = false;
  while ((opt = getopt_long(argc, argv, "t:v:f:o:?", long_options, NULL)) !=
         EOF) {
    switch (opt) {
    case 't': {
      numThreads = atoi(optarg);
      break;
    }
    case 'v': {
      viewIndex = atoi(optarg);
      // change view settings
      if (viewIndex < 0 || viewIndex >= VIEWCNT) {
        fprintf(stderr, "Invalid view index %d\n", viewIndex);
        return 1;
      }
      break;
    }
    case 'f': {
      if (sscanf(optarg, "%f:%f:%f:%f", &x0, &y0, &x1, &y1) != 4) {
        fprintf(stderr, "Couldn't extract field from '%s'\n", optarg);
        exit(1);
      }
      break;
    }
    case 'o': {
      strcpy(fname, optarg);
      have_file = true;
      break;
    }
    case '?':
    default:
      usage(argv[0]);
      return 1;
    }
  }
  // end parsing of commandline options
  float scaleValue = scaleValues[viewIndex];
  float shiftX = shiftXs[viewIndex];
  float shiftY = shiftYs[viewIndex];
  scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

  if (have_file) {
    // In this mode, assume goal is to simply generate the output as fast as
    // possible
    int *output = new int[width * height];
    //
    typedef WorkerArgs* WAPtr;
    WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numThreads);
    for (int i = 0; i < numThreads; i++) {
        args[i] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
    }


    int start_row = 0, decomp_height;
    div_t rc = div(height + 1, numThreads);
    decomp_height = rc.quot;
      for (int i = 0; i < numThreads; i++) {
            /*
            * Accomodate stop counts (only the last
            * thread will have to do some extra work).
            */
            if ((i + 1) == numThreads) {
                decomp_height = rc.quot + rc.rem;
            }

            args[i]->start_row = start_row;
            args[i]->num_rows = decomp_height;
            start_row += rc.quot;

            /* Populate the argument list. */
            args[i]->threadId = i;
            args[i]->x0 = x0;
            args[i]->y0 = y0;
            args[i]->x1 = x1;
            args[i]->y1 = y1;
            args[i]->width = width;
            args[i]->height = height;
            args[i]->maxIterations = maxIterations;
            args[i]->threadId = i;
            args[i]->numThreads = numThreads;
            args[i]->output = output;
            args[i]->thd_exec_time = -1;
        }


    threadPool_PerThread threadPool(numThreads);
    threadPool.submit(&workerThreadStart, args, numThreads);
    threadPool.dispatch();
    //
    writePPMImage(output, width, height, fname, maxIterations);
    delete[] output;
    exit(0);
  }

  int *output_serial = new int[width * height];
  int *output_thread = new int[width * height];

  //
  // Run the serial implementation.  Run the code numRuns times and
  // take the minimum to get a good estimate.
  //
  int numRuns = 1;
  memset(output_serial, 0, width * height * sizeof(int));
  //double minSerial = 1e30;
    int64_t minSerial = 0;
  for (int i = 0; i < numRuns; ++i) {
    //double startTime = CycleTimer::currentSeconds();
    mandelbrotSerial(x0, y0, x1, y1, width, iheight, 0, iheight, maxIterations,
                     output_serial);
    //double endTime = CycleTimer::currentSeconds();
    //minSerial = std::min(minSerial, endTime - startTime);
  }

  printf("[mandelbrot serial]:\t\t[%.3f] ms\n", minSerial * 1000);
  sprintf(fname, "mandelbrot-v%d-serial.ppm", viewIndex);
  writePPMImage(output_serial, width, height, fname, maxIterations);

  //
  // Run the threaded version
  //
  printf("running threaded version\n");
  memset(output_thread, 0, width * height * sizeof(int));
  double minThread = 1e30;
  for (int k = 0; k < numRuns; ++k) {
        printf("Running iteration: %d\n", k);
        //double startTime = CycleTimer::currentSeconds();
        //
        typedef WorkerArgs* WAPtr;
        WAPtr* args = (WAPtr*) malloc(sizeof(WorkerArgs*)*numThreads);
        for (int j = 0; j < numThreads; j++) {
            args[j] =  (WorkerArgs*) malloc(sizeof(WorkerArgs));
        }

        int start_row = 0, decomp_height;
        div_t rc = div(height + 1, numThreads);
        decomp_height = rc.quot;
        for (int i = 0; i < numThreads; i++) {
            /*
            * Accomodate stop counts (only the last
            * thread will have to do some extra work).
            */
            if ((i + 1) == numThreads) {
                decomp_height = rc.quot + rc.rem;
            }

            args[i]->start_row = start_row;
            args[i]->num_rows = decomp_height;
            start_row += rc.quot;

            /* Populate the argument list. */
            args[i]->threadId = i;
            args[i]->x0 = x0;
            args[i]->y0 = y0;
            args[i]->x1 = x1;
            args[i]->y1 = y1;
            args[i]->width = width;
            args[i]->height = height;
            args[i]->maxIterations = maxIterations;
            args[i]->threadId = i;
            args[i]->numThreads = numThreads;
            args[i]->output = output_thread;
            args[i]->thd_exec_time = -1;
        }


        threadPool_PerThread threadPool(numThreads);
        threadPool.submit(&workerThreadStart, args, numThreads);
        threadPool.dispatch();
        // busy wait until all threads are finished
        //while(!threadPool.isAllTasksFinished);
        //sleep(10);
        printf("dispatch done iteration: %d\n", k);
  }

  printf("[mandelbrot thread]:\t\t[%.3f] ms\n", minThread * 1000);
  sprintf(fname, "mandelbrot-v%d-thread-%d.ppm", viewIndex, numThreads);
  writePPMImage(output_thread, width, height, fname, maxIterations);

  if (!verifyResult(output_serial, output_thread, width, height)) {
    printf("ERROR : Output from threads does not match serial output\n");

    delete[] output_serial;
    delete[] output_thread;

    return 1;
  } else {
    printf("output matches with serial\n");
  }

  // compute speedup
  printf("++++\t\t\t\t(%.2fx speedup from %d threads)\n", minSerial / minThread,
         numThreads);

  delete[] output_serial;
  delete[] output_thread;

  return 0;
}
