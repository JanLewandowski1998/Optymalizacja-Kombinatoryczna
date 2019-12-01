#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "generator.h"

class Instance
{
private:
    int m_procsCount = -1;
    int m_tasksCount = -1;
    bool m_withOptimal = false;
    int m_optimal = -1;
    std::vector<int> m_tasks;

    int m_score = -1;


private:
    int m_intPower(int base, int exp)
    {
        int result = 1;
        while (exp)
        {
            result *= base;
            exp--;
        }
        return result;
    }

    int m_strToInt(std::string nrStr)
    {
        int nr = 0;
        int length = nrStr.length();
        for (int i = 0; i < length; i++)
        {
            int digit = nrStr[i] - 48;
            int power = m_intPower(10, length - i - 1);
            nr += digit * power;
        }
        return nr;
    }

public:
    void readFromFile(std::string fileName = "instance.txt", bool withOptimal = false)
    {
        m_withOptimal = withOptimal;

        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in);

        std::string line;
        if (withOptimal)
        {
            getline(file, line);
            if (line[0] != '#')
            {
                std::cout << "ERROR: the first line of the\n"
                          << "instance file does not start with\n"
                          << "'#' (withOptimal = true).\n";
                exit(0);
            }
            std::string optimalStr = "";
            int length = line.length();
            for (int i = 2; i < length; i++)
            {
                optimalStr += line[i];
            }
            m_optimal = m_strToInt(optimalStr);
        }

        file >> m_procsCount;
        file >> m_tasksCount;

        while(!file.eof())
        {
            int duration;
            file >> duration;
            m_tasks.push_back(duration);
        }
    }

    void solve()
    {

    }

    int getProcsCount() {return m_procsCount;}

    int getTasksCount() {return m_tasksCount;}

    int getOptimal() {return m_optimal;}

    int getScore() {return m_score;}

    bool isWithOptimal() {return m_withOptimal;}

    std::vector<int>& getTasks() {return m_tasks;}
};

void greedy()
{
    Instance instance;
    instance.readFromFile("instance.txt", true);

    std::cout << "processors: " << instance.getProcsCount()
              << ", tasks: " << instance.getTasksCount() << '\n';

    instance.solve();

    std::cout << "score: " << instance.getScore();
    if (instance.isWithOptimal()) std::cout << "/" << instance.getOptimal() << "\n\n";
}

void run()
{
    for (int i = 1; i <= 10; i++)
    {
        generateInstance(i * 10, i * 10, i * 50, i * 50, 100, true);
        greedy();
    }
}

int main()
{
    srand(time(NULL));
    run();
    return 0;
}
