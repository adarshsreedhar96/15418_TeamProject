# 15418_TeamProject
This project includes different implementation of work stealing logic, with dynamic scheduling.

There are three primary layers/files of code here: The ThreadPool and Queue layer, The test benchmark layer, and the layer containing the worker tasks.

The ThreadPool and Queue creates a pool of threads (as requested by the user), creates a queue, and enqueues the tasks submitted. We support two types of queues, one a centralized queue from which all threads must grab tasks from, and a queue per thread, where locking is not required since each thread pulls from its own queue. Th allocation of tasks onto the queues is currently round-robin, however we welcome changes to include a custom function that can suitably distribute tasks on the queues.

The test benchmark layer is where we test different types of work stealing algorithms, which queue type to use and many more options. We include the timer functions here for easy comparison of performance. To write a new benchmark, simply extend the BenchMark class, and implement all its methods. 

The Fields and Methods of class BenchMark, explained:
function::task - This is the function pointer that we want to execute across different threads. Every Task runs this function with a different set of inputs.
getTasks - This is where the total input range is split into smaller chunks, each of which would be executed by a task.
workerTask - The actual task that is to be run in parallel for different tasks across different threads.

We have four benchmarks present in this repository:
* MatrixMul - A simple matrix multiplication logic for variable sized matrices. 
* Mandelbrot - This implements the Mandelbrot function
* RandomSleep - This implements a random sleep logic (pseudorandom i.e. deterministic) across different threads to simulate a perfectly uneven distribution of workload.
* Text Search - This takes in a bag of text, and parallely searches for a word. The idea is simulate a one-result search parallely, and also be able to stop once found (unlike others where all threads need to complete).

We have three types of Queueing Logic:
* Single Queue - All threads need to grab tasks from here
* Per-Thread Queue - All threads get their own queues
* Priority Queue - To give flexibility for user to give custom comparator functions for parallel-search usecases.

We have two types of Stealing Logic:
* Neighbour Steal - We go across the queues of threads in a sequential manner and search for tasks to steal. We stop when we find a task.
* Random Steal - We randomly choose a que to steal tasks from.

We have three types of granularitites of work stealing:
* Steal One Task
* Steal Half Tasks - This steals half the tasks present in the queue
* Steal All Tasks

As mentioned, we have four benchmarks:
* test_mandelbrot
* test_randomsleep
* test_matrixmul
* test_textsearch

These are the files to be run. Command:
g++ -std=c++11 test_randomsleep.cpp -lpthread -lm
./a.out

Parameters to change:
* The numberOfTasks and numberOfThreads field present in each of these classes can be changed to anything as required.
* To switch between centralized and per_thread queues, set/unset the macro CENTRALIZED as required in each test file.
* To switch stealing on/off set the bool value in the second parameter of the constructor function as true/falase respectively. This applies only to per_thread queue. The third and fourth parameter do not matter when this value is false.
* To change the granularity of the tasks to be stolen (possible values: STEALONETASK, STEALHALFTASKS, STEALALLTASKS), set the third parameter of the constructor call.
* To change the type of work stealing (possible values: STEALNEIGHBOURTASK, STEALRANDOMTASK), change the last parameter of the constructor call. 

