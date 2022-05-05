#include "benchmark.h"
#include <string.h>
#include <vector>

using namespace std;
class Search
{

private:
    std::string text;
    std::string target;
    std::vector<std::string> tokenizedText;
    int wordcount;

public:
    typedef struct
    {
        std::string target;
        int start_idx;
        int end_idx;
        bool found;
        std::vector<std::string> text;
    } search;

    Search()
    {
    }

    void setInput(std::string text, std::string target)
    {
        this->target = target;
        this->text = text;
        tokenize(text, " ");
        this->wordcount = this->tokenizedText.size();
    }

    void tokenize(string s, string del = " ")
    {
        int start = 0;
        int end = s.find(del);
        while (end != -1)
        {
            tokenizedText.push_back(s.substr(start, end - start));
            start = end + del.size();
            end = s.find(del, start);
        }
        tokenizedText.push_back(s.substr(start, end - start));
    }

    static bool workerTask(void *threadArgs)
    {
        // printf("workerThreadStart called by thread: %d\n", std::this_thread::get_id());

        search *args = static_cast<search *>(threadArgs);
        printf("working on %d to %d\n", args->start_idx, args->end_idx);
        for (int i = args->start_idx; i < args->end_idx; i++)
        {
            if (args->text[i] == args->target)
            {
                args->found = true;
                return true;
            }
        }
        return false;
    }
    template <typename T>
    void getTasks(T **args, int numberOfTasks)
    {
        int size = this->tokenizedText.size();
        for (int i = 0; i < numberOfTasks; i++)
        {
            args[i] = new search();
            args[i]->text = this->tokenizedText;
            args[i]->target = this->target;
            args[i]->start_idx = i * (size / numberOfTasks);
            args[i]->end_idx = (i + 1) * (size / numberOfTasks);
        }
        args[numberOfTasks - 1]->end_idx = size;
    }

    int *getPriority(int numberOfTasks, int numberOfSections, int *sectionPriority)
    {
        int *priority = (int *)malloc(sizeof(int) * numberOfTasks);

        int section = -1;
        for (int i = 0; i < numberOfTasks; i++)
        {
            if (numberOfTasks % numberOfSections == 0)
                section++;
            priority[i] = sectionPriority[section];
        }
        return priority;
    }
};