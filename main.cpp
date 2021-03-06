#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "generator.h"

struct Task
{
public:
    int duration;
    int start;
    int finish;
    int proc;

public:
    Task(int d = -1, int s = -1, int f = -1, int p = -1)
    : duration(d), start(s), finish(f), proc(p) {}
};

std::ostream& operator<<(std::ostream& stream, const Task& task)
{
    return stream << task.duration << ", "
                  << task.start    << ", "
                  << task.finish   << ", "
                  << task.proc;
}

template<typename T>
void printVec(const std::vector<T>& vec)
{
    std::cout << '\n';
    for (int i = 0; i < vec.size(); i++) std::cout << vec[i] << '\n';
}

class Instance
{
private:
    int m_procsCount = -1;
    int m_tasksCount = -1;
    bool m_withOptimal = false;
    int m_optimal = -1;
    std::vector<int> m_tasks;

    int m_score = -1;
    std::vector<std::vector<Task>> m_solution;


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

    void m_insert_keep_sorted(std::vector<int>& vec, int value)
    {
        vec.push_back(value);
        for (int i = vec.size() - 1; i > 0; i--)
        {
            if (vec[i] > vec[i - 1])
            {
                int temp = vec[i];
                vec[i] = vec[i - 1];
                vec[i - 1] = temp;
            }
            else break;
        }
    }

public:
    void readFromFile(std::string fileName = "instance.txt")
    {
        m_solution.clear();

        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in);

        std::string line;
        getline(file,line);
        if (line[0] == '#')
        {
            m_withOptimal = true;
            std::string optimalStr = "";
            int length = line.length();
            for (int i = 2; i < length; i++)
            {
                optimalStr += line[i];
            }
            m_optimal = m_strToInt(optimalStr);
        }
        else
        {
            m_withOptimal = false;
            file.clear();
            file.seekg(0, std::ios::beg);
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

    void solveGreedy()
    {
        m_solution.clear(); //Czyszczenie solucji:
        m_solution.resize(m_procsCount);

        std::vector<int> tasksLeft = m_tasks; //Vector zawierający pozostałe zadania:
        std::sort(tasksLeft.begin(), tasksLeft.end()); //Sortowanie:

        std::vector<Task> tasksRunning; //Vector zawierający wykonujące się zadania:

        std::vector<int> timestamps; //Vector zawierający istotne punkty czasowe:
        timestamps.push_back(0); //Pierwszym istotnym momentem jest czas zerowy:

        std::vector<int> freeProcs; //Vector zawierający aktualnie wolne procesory:
        freeProcs.resize(m_procsCount); //Zapełnianie freeProcs kolejnymi liczbami całkowitymi dodatnimi:
        for (int i = 0; i < m_procsCount; i++)
        {
            freeProcs[i] = i;
        }

        while (!tasksLeft.empty()) //Pętla wykunująca się aż do wyczerpania zadań:
        {
            int now = timestamps[timestamps.size() -  1]; //Pobranie aktualnego punktu czasowego:
            timestamps.pop_back(); //Usunięcie go z timestamps:

            for (int i = 0; i < tasksRunning.size(); i++) //Iteracja przez wszystkie wykonywane zadania celem znalezienia ukończonych w danym momencie czasowym:
            {
                if (tasksRunning[i].finish == now) //Spr. czy dane zadanie właśnie się zakończyło:
                {
                    freeProcs.push_back(tasksRunning[i].proc); //Dodanie procesora zakończonego zadania do vectora wolnych procesorów:
                    tasksRunning.erase(tasksRunning.begin() + i); //Usunięcie zadania z tasksRunning:
                    i--;
                }
            }

            while (freeProcs.size() > 0 && tasksLeft.size() > 0)
            {
                tasksRunning.push_back({tasksLeft[tasksLeft.size() - 1],
                                        now,
                                        now + tasksLeft[tasksLeft.size() - 1],
                                        freeProcs[freeProcs.size() - 1]});

                int p = tasksRunning[tasksRunning.size() - 1].proc;
                m_solution[p].push_back(tasksRunning[tasksRunning.size() - 1]);
                if (m_solution[p][m_solution[p].size() - 1].finish > m_score) m_score = m_solution[p][m_solution[p].size() - 1].finish;
                m_insert_keep_sorted(timestamps, now + tasksLeft[tasksLeft.size() - 1]);
                tasksLeft.pop_back();
                freeProcs.pop_back();
            }
        }
    }

	void swapTasks(int proc1, int t1, int proc2, int t2)
	{
		Task task1 = m_solution[proc1][t1];
		Task task2 = m_solution[proc2][t2];
		task1.proc = proc2;
		task2.proc = proc1;
		m_solution[proc1].erase(m_solution[proc1].begin() + t1);
		m_solution[proc2].erase(m_solution[proc2].begin() + t2);

		for (int i = t1; i < m_solution[proc1].size(); i++)
		{
			Task &tmp = m_solution[proc1][i];

			if (i == 0)
				tmp.start = 0;
			else
				tmp.start = m_solution[proc1][i - 1].finish;

			tmp.finish = tmp.start + tmp.duration;
		}
		for (int i = t2; i < m_solution[proc2].size(); i++)
		{
			Task &tmp = m_solution[proc2][i];

			if (i == 0)
				tmp.start = 0;
			else
				tmp.start = m_solution[proc2][i - 1].finish;

			tmp.finish = tmp.start + tmp.duration;
		}

		Task tmp = m_solution[proc1].back();
		task2.start = tmp.finish;
		task2.finish = task2.duration + task2.start;
		m_solution[proc1].push_back(task2);

		tmp = m_solution[proc2].back();
		task1.start = tmp.finish;
		task1.finish = task1.duration + task1.start;
		m_solution[proc2].push_back(task1);

		m_score = (m_solution[0].back()).finish;
		for (int i = 1; i < m_solution.size(); i++)
			if (m_score < (m_solution[i].back()).finish)
				m_score = (m_solution[i].back()).finish;
	}

	bool solveLocalSearch()
	{
		//Find procsFinishedLast and procsFinishedFirst
		std::vector<int> procsFinishedLast;
		std::vector<int> procsFinishedFirst;
		int firstsTime = -1;
		for (int i = 0; i < m_solution.size(); i++)
		{
			Task lastTask = m_solution[i].back();
			if (firstsTime == -1) 
			{
				procsFinishedFirst.push_back(i);
				firstsTime = lastTask.finish;

				if (lastTask.finish == m_score)
					procsFinishedLast.push_back(i);
			}
			else
			{
				if (lastTask.finish < firstsTime)
				{
					firstsTime = lastTask.finish;
					procsFinishedFirst = { i };
				}
				else if (lastTask.finish == firstsTime)
				{
					procsFinishedFirst.push_back(i);
				}

				if (lastTask.finish == m_score)
				{
					procsFinishedLast.push_back(i);
				}
			}
		}

		//Find if there is any usefull tasks shift possible
		for (int i = 0; i < procsFinishedFirst.size(); i++)
		{
			int firstProc = procsFinishedFirst[i];
			int firstProcEndTime = (m_solution[firstProc].back()).finish;
			for (int j = 0; j < procsFinishedLast.size(); j++)
			{
				int lastProc = procsFinishedLast[j];
				int lastProcEndTime = (m_solution[lastProc].back()).finish;
				for (int firstTask = 0; firstTask < m_solution[firstProc].size(); firstTask++)
				{
					int firstTaskTime = m_solution[firstProc][firstTask].duration;
					for (int lastTask = 0; lastTask < m_solution[lastProc].size(); lastTask++)
					{
						int lastTaskTime = m_solution[lastProc][lastTask].duration;
						int newFirstProcEndTime = firstProcEndTime - firstTaskTime + lastTaskTime;
						int newLastProcEndTime = lastProcEndTime - lastTaskTime + firstTaskTime;

						if (newFirstProcEndTime < lastProcEndTime &&
							newLastProcEndTime < lastProcEndTime)
						{
							swapTasks(firstProc, firstTask, lastProc, lastTask);
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}

    void printInstance()
    {
        if (m_withOptimal) std::cout << "# " << m_optimal << '\n';
        std::cout << m_procsCount << '\n' << m_tasksCount << '\n';
        for (int i = 0; i < m_tasksCount; i++)
        {
            std::cout << m_tasks[i] << '\n';
        }
    }

    void printSolution()
    {
        for (int i = 0; i < m_procsCount; i++)
        {
            int s = m_solution[i].size();
            for (int j = 0; j < s; j++)
                std::cout << m_solution[i][j] << '\n';
        }
    }

    int getProcsCount() {return m_procsCount;}

    int getTasksCount() {return m_tasksCount;}

    int getOptimal() {return m_optimal;}

    int getScore() {return m_score;}

    bool isWithOptimal() {return m_withOptimal;}

    std::vector<int>& getTasks() {return m_tasks;}

    std::vector<std::vector<Task>>& getSolution() {return m_solution;}
};

void greedy(bool showWeakOnly = false, std::string path = "instance.txt")
{
    static int count = 1;

    Instance instance;
    instance.readFromFile(path.c_str());

    instance.solveGreedy();

    if (showWeakOnly)
    {
        if (!(((double) instance.getScore() / instance.getOptimal() * 100.0) > 103.0))
        {
            std::cout << count++ << " ";
            return;
        }
        count++;
    }

    //instance.printInstance();

    std::cout << "\nprocessors: " << instance.getProcsCount()
              << ", tasks: " << instance.getTasksCount() << '\n';

    std::cout << "score: " << instance.getScore();
    if (instance.isWithOptimal()) 
		std::cout << ", optimal: " << instance.getOptimal() << ", (%): " << (double) instance.getScore() / instance.getOptimal() * 100 << "%";
	std::cout << std::endl;
    //instance.printSolution();

    std::cout << "\n-----------------------------\n";
}

void localSearch(bool showWeakOnly = false, std::string path = "instance.txt")
{
	static int count = 1;

	Instance instance;
	instance.readFromFile(path.c_str());

	instance.solveGreedy();
	for (; instance.solveLocalSearch(); )
		continue;
	if (showWeakOnly)
	{
		if (!(((double)instance.getScore() / instance.getOptimal() * 100.0) > 103.0))
		{
			std::cout << count++ << " ";
			return;
		}
		count++;
	}

	//instance.printInstance();

	std::cout << "\nprocessors: " << instance.getProcsCount()
		<< ", tasks: " << instance.getTasksCount() << '\n';

	std::cout << "score: " << instance.getScore();
	if (instance.isWithOptimal())
		std::cout << ", optimal: " << instance.getOptimal() << ", (%): " << (double)instance.getScore() / instance.getOptimal() * 100 << "%";
	std::cout << std::endl;
	//instance.printSolution();

	std::cout << "\n-----------------------------\n";
}

void random()
{
    for (int i = 1; i <= 5; i++)
    {
        generateInstance(1000, 2000, 10000, 20000, 1000, true);
		std::cout << "greedy: " << std::endl;
		greedy(false);
		std::cout << "localSearch: " << std::endl;
		localSearch(false);
    }
}

void ranked()
{
    const int nr = 5;
    std::string paths[nr] = {"ranked/m25.txt", "ranked/m50.txt", "ranked/m50n1000.txt", "ranked/m50n200.txt", "ranked/m10n200.txt"};

    for (int i = 0; i < nr; i++)
    {
		std::cout << "greedy: " << std::endl;
		greedy(false, paths[i]);
		std::cout << "localSearch: " << std::endl;
        localSearch(false, paths[i]);
    }
}

int main()
{
    srand(time(NULL));
	ranked();
    return 0;
}
