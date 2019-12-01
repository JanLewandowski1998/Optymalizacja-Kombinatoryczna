#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
#include "generator.h"

//#define BOUND_PROCS_LOW 3 //10
//#define BOUND_PROCS_HIGH 10000 //1000
//#define BOUND_TASKS_LOW 2 //2
//#define BOUND_TASKS_HIGH 200 //200
//#define OPTIMAL_FACTOR 1000 //100

int ceiling(int nr, int div)
{
    if (nr % div) return (nr / div) + 1;
    else return (nr / div);
}

void generateInstance(int procsLow,           //Lower bound for the number of processors
                      int procsHigh,          //Upper [...]
                      int tasksLow,           //Lower bound fot the number of tasks
                      int tasksHigh,          //Upper [...]
                      int optimalFactor,      //Factor which the minimal optimal time is multiplyed by
                      bool nrOfTasksManually) //If the lower and upper bound for the number of tasks should be 'lowerTasksBound = procsCount * tasksLow;' or 'lowerTasksBound = tasksLow;'
{
    //std::cout << "Generating an instance...\n\n";

    //Bounds for number of processors:
    int lowerProcsBound = procsLow;
    int upperProcsBound = procsHigh;

    //Number of processors:
    const int procsCount = (rand() % (upperProcsBound - lowerProcsBound + 1)) + lowerProcsBound;
    //std::cout << "Processors: " << procsCount << '\n';

    //Bounds for number of tasks (depending on number of processors):
    int lowerTasksBound;
    int upperTasksBound;
    if (nrOfTasksManually)
    {
        lowerTasksBound = tasksLow;
        upperTasksBound = tasksHigh;
    }
    else
    {
        lowerTasksBound = procsCount * tasksLow;
        lowerTasksBound = procsCount * tasksHigh;
    }

    //Number of tasks:
    const int tasksCount = (rand() % (upperTasksBound - lowerTasksBound + 1)) + lowerTasksBound;
    //std::cout << "Tasks: " << tasksCount << '\n';

    //Bounds for optimal time:
    int lowerOptimalBound = ceiling(tasksCount, procsCount);
    int upperOptimalBound = lowerOptimalBound * optimalFactor;

    //Optimal time:
    int optimalTime = (rand() % (upperOptimalBound - lowerOptimalBound + 1)) + lowerOptimalBound;
    //std::cout << "Optimal time: " << optimalTime << '\n';

    //Array where True(1) means that at this point a task starts:
    bool** timestamps = new bool*[procsCount];
    for (int i = 0; i < procsCount; i++)
        timestamps[i] = new bool[optimalTime];

    //Filling the array with 0s:
    for (int i = 0; i < procsCount; i++)
        for (int j = 0; j < optimalTime; j++)
            timestamps[i][j] = 0;

    //Tasks to be yet used:
    int tasksLeft = tasksCount;

    //There are [procsCount] number of tasks that must start at 0 time:
    for (int i = 0; i < procsCount; i++)
    {
        timestamps[i][0] = 1;
        tasksLeft--;
    }

    //Creating vector of available availableTimestamps:
    std::vector<std::vector<int>> availableTimestamps;
    availableTimestamps.resize(procsCount);
    for (int i = 0; i < procsCount; i++)
        for (int j = 1; j < optimalTime; j++) availableTimestamps[i].push_back(j);

    //filling tasks array with remaining tasks:
    while (tasksLeft)
    {
        int row = rand() % procsCount;
        if (availableTimestamps[row].empty()) continue;
        int col = rand() % availableTimestamps[row].size();

        timestamps[row][availableTimestamps[row][col]] = 1;
        availableTimestamps[row].erase(availableTimestamps[row].begin() + col);

        tasksLeft--;
    }

    //Vector for durations of the tasks:
    std::vector<std::vector<int>> durations;
    durations.resize(procsCount);

    //Converting timestamps to durations:
    for (int i = 0; i < procsCount; i++)
    {
        int duration = 1;
        for (int j = 1; j < optimalTime; j++)
        {
            if (timestamps[i][j])
            {
                durations[i].push_back(duration);
                duration = 1;
            }
            else
            {
                duration++;
            }
        }
        durations[i].push_back(duration);
    }

    //Vector to randomly shuffle the tasks:
    std::vector<int> shuffle;

    //Checking if each row sums up to optimalTime:
    for (int i = 0; i < procsCount; i++)
    {
        int sum = 0;
        for (int j = 0; j < durations[i].size(); j++)
        {
            sum += durations[i][j];
            shuffle.push_back(durations[i][j]);
        }
        if (sum != optimalTime)
        {
            std::cout << "ERROR: sum of tasks durations from processor "
                      << i << ". equals " << sum
                      << " while it should be equal to "
                      << optimalTime << '\n';
            exit(0);
        }
    }
    //std::cout << "\nSums of tasks durations for each\n"
              //<< "processor are equal to optimal time = "
              //<< optimalTime << '\n';

    //Random shuffle:
    int size = shuffle.size();
    for (int i = 0; i < size; i++)
    {
        int a = rand() % size;
        int b = rand() % size;
        int temp = shuffle[a];
        shuffle[a] = shuffle[b];
        shuffle[b] = temp;
    }

    //Output file:
    std::ofstream file;
    file.open("instance.txt", std::ios::out);

    //Exporting data:
    //std::cout << "\nExporting data...\n";
    file << "# " << optimalTime << std::endl;
    file << procsCount << std::endl;
    file << tasksCount << std::endl;
    for (int i = 0; i < size - 1; i++)
        file << shuffle[i] << std::endl;
    file << shuffle[size - 1];

    //Deleting tasks array:
    for (int i = 0; i < procsCount; i++)
        delete [] timestamps[i];
    delete [] timestamps;

    file.clear();
    file.close();

    //std::cout << "\nGenerating finished.\n---------------------------------------------------";
}
