//#include "stdafx.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static int maxJobs;
static int maxNodes;
static int realHowMany;

static int debugger = 0;

static vector<vector<int>> tasks; //VECTOR WITH TASKS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double countTime(clock_t t) //TIME CONVERTION TO SECONDS
{
    return static_cast<double>(t) / CLOCKS_PER_SEC;
}

void exportSolution(vector<vector<int>> tab, string fileName) //EXPORT SOLUTION TO .txt
{
    ofstream fileOut;
    fileOut.open(fileName.c_str());
    for (int j = 0; j < tab[0].size(); j++)
        fileOut << tab[0][j] << " ";
    for (int i = 1; i < tab.size(); i++)
    {
        fileOut << endl;
        for (int j = 0; j < tab[i].size(); j++)
        {
            fileOut << tab[i][j] << " ";
            //if (j == 0) cout << "ID: " << tab[i][j] << endl;
        }
    }
    fileOut.close();
    fileOut.clear();
}

void printVector(vector<vector<int>> tab) //PRINT VECTOR
{
    cout << "\nsize: " << tab.size() << endl;
    for (int i = 0; i < tab.size(); i++)
    {
        for (int j = 0; j < 5; j++)
            cout << tab[i][j] << " ";
        cout << endl;
    }
}

void readTasks(int howMany, string fileName) //READING TEST FILE, PUTTING DATA INTO VECTOR tasks
{
    fstream f;
    f.open(fileName.c_str());

    if (!f.good()) //kontrola czy plik istnieje
    {
        cout << "\nTEXT FILE ERROR - FILE NOT FOUND\n";
        exit(0);
    }

    for (int i = 0; !f.eof() && i < howMany; i++)
    {
        string s;
        getline(f, s); //cout << endl << "POBRANA LINIA: '" << s << "'" << endl << "maxNodes = " << maxNodes << ", maxJobs = " << maxJobs << endl; cin >> debugger;

        //ignores lines with ';' at the beggining, but catches MaxJobs and MaxNodes
        if (s[0] == ';')
        {
            int c = 1;
            string s1 = "";
            for (; s[c] == ' ' && c < s.length(); c++)
            {
            }
            for (; s[c] != ' ' && c < s.length(); c++)
                s1 += s[c];

            if (s1 == "MaxJobs:")
            {
                s1 = "";
                for (int j = c + 1; s[j] != '\0'; j++)
                    s1 += s[j];
                maxJobs = stoi(s1);
                if (maxJobs < howMany)
                {
                	cout << endl << "Nie ma tylu zadan w pliku z instancja!\nPrzyjmuje [maxJobs - liczba niepoprawnych zadan] jako liczbe zadan do uszeregowania." << endl << endl;
                	howMany = maxJobs;
				}
            }
            if (s1 == "MaxProcs:")
            {
                s1 = "";
                for (int j = c + 1; s[j] != '\0'; j++)
                    s1 += s[j];
                maxNodes = stoi(s1);
            }

            i--;
            continue;
        }
        //cout << "\nERROR: " << maxJobs << "; " << maxNodes << endl;
        tasks.push_back({});
        int c = 0; //cout << endl;
        for (int j = 0; j < 5; j++)
        {
            string numberS;
            for (; std::isspace(s[c]); c++)
            {
            }
            for (; !std::isspace(s[c]); c++)
                numberS += s[c];

            //if (j == 0) cout << endl << numberS << endl;

            if ((j == 3 && stoi(numberS) <= 0) ||
                (j == 4 && stoi(numberS) > maxNodes) ||
                (j == 4 && stoi(numberS) <= 0) ||
                (j == 0 && stoi(numberS) < 0) ||
                (j == 1 && stoi(numberS) < 0)) //!!!
            {
                tasks.erase(tasks.begin() + i);
                i--;
                maxJobs--;
                if (maxJobs < howMany) howMany--;
                break;
            }
            (tasks.at(i)).push_back(stoi(numberS));
        }
        if (i < tasks.size())
            (tasks.at(i)).at(2) = -1;
    }
    f.close();
    f.clear();
    
    realHowMany = howMany;

    if (howMany > tasks.size())
        exit(0);
}

int durationLast(vector<vector<int>> tab) //SOLUTION DURATION MEASUREMENT
{
    int lastFinishTaskTime = -1;
    for (int i = 0; i < tab.size(); i++)
        if (lastFinishTaskTime < tab[i][2])
            lastFinishTaskTime = tab[i][2];

    return lastFinishTaskTime;
}

double duration(vector<vector<int>> tab) //SOLUTION DURATION MEASUREMENT
{
    double sum = 0;
    for (int i = 0; i < tab.size(); i++)
        sum += tab[i][2];

    return sum;
}

double durationNotConverted(vector<vector<int>> tab) //SOLUTION DURATION MEASUREMENT
{
    double sum = 0;
    for (int i = 0; i < tab.size(); i++)
        sum += (tab[i][2] + tab[i][3]);

    return sum;
}

void importTasksAppearanceTimes(vector<vector<int>> tab, vector<int> &timeV) //IMPORT CZASĂ“W POJAWIENIA SIĘ ZADAĹ DO VECTORA
{
    for (int i = 0; i < tab.size(); i++)
        timeV.push_back(tab[i][1]);
}

void takeSmallestTimeValue(vector<int> &importantTimeValues, int &time) //POBIERA NAJMNIEJSZY CZAS Z importantTimeValues DO time I USUWA GO Z importantTimeValues
{
    if (!importantTimeValues.empty())
    {
        time = importantTimeValues[0];
        int tmp = 0;
        for (int i = 1; i < importantTimeValues.size(); i++) //PRZESZUKIWANIE LINIOWE
            if (importantTimeValues[i] < time)
            {
                time = importantTimeValues[i];
                tmp = i;
            }
        importantTimeValues.erase(importantTimeValues.begin() + tmp);
    }
}

int doTask(vector<int> &task, vector<vector<int>> &ans, vector<vector<int>> &ongoingTasks, int &time, int &availableNodes, vector<int> &importantTimeValues)
{
    int endTime;

    //wait enough time and do task
    for (; 1;)
    {
        if (task.at(1) <= time && task.at(4) <= availableNodes)
        {
            task.at(2) = time;
            endTime = task[2] + task[3];
            importantTimeValues.push_back(endTime);
            ongoingTasks.push_back(task);
            ans.push_back(task);
            availableNodes -= task.at(4);
            break;
        }
        else
        {
            takeSmallestTimeValue(importantTimeValues, time);

            //releasing nodes and deleting done tasks from ongoingTasks
            for (int j = 0; j < ongoingTasks.size(); j++)
                if ((ongoingTasks.at(j)).at(3) == time - (ongoingTasks.at(j)).at(2))
                {
                    availableNodes += (ongoingTasks.at(j)).at(4);
                    ongoingTasks.erase(ongoingTasks.begin() + j);
                    j--;
                }
        }
    }
    return endTime;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<int>> greedyTaskDuration(vector<vector<int>> tab, int key1, int key2, string mode, string operand, bool convert) //********************GREEDY ALGORITHM BEGINNING!!!**************************
{
	bool sort;
	
    int availableNodes = maxNodes;
    vector<vector<int>> readyTasks;   //MOŻNA JUŻ ZACZĄĆ WYKONYWAĆ
    vector<vector<int>> ongoingTasks; //WYKONUJĄCE SIĘ ZADANIA
    vector<vector<int>> greedySolution;
    vector<int> importantTimeValues; /*w important time values przechowujemy te
    jednostki czasu w których cokolwiek się dzieje, tzn. przyjmowane jest nowe
    zadanie do readyTasks lub kończone jest wykonywanie jakiegoś zadania*/
    vector<int> freeProcessors;      //list of available processors <0 ; (maxNodes - 1))
    for (int i = 0; i < maxNodes; i++)
    {
        freeProcessors.push_back(i);
    }
    int time;

    importTasksAppearanceTimes(tab, importantTimeValues); //IMPORT APPEARANCE TASKS TIMES TO importantTimeValues VECTOR
    while (!tab.empty() || !readyTasks.empty())
    {
    	sort = false;
        takeSmallestTimeValue(importantTimeValues, time); //time = takeSmallestTimeValue(importantTimeValues);

        //releasing nodes and deleting done tasks from ongoingTasks
        for (int i = 0; i < ongoingTasks.size(); i++)
            if (ongoingTasks[i][3] == time - ongoingTasks[i][2]) //TASK DURATION == NOW - START TIME - sprawdzenie czy zadanie właśnie się skończyło
            {
                availableNodes += ongoingTasks[i][4]; //UP: jeśli tak to zwiększamy liczbę dostępnych procesorów, bo zadanie skończyło się wykonywaÄ‡
                if (convert)
                {
                    for (int j = 0; j < ongoingTasks[i][4]; j++)
                    {
                        freeProcessors.push_back(ongoingTasks[i][j + 5]); //Dodajemy do listy wolnych procesorów te zwolnione przez zadanie, które sie zakończyło
                    }
                }
                ongoingTasks.erase(ongoingTasks.begin() + i); //i usuwamy to zadanko z ongoing tasks
                i--;
            }
        //moving ready tasks to readyTasks from tasks
        for (int i = 0; i < tab.size(); i++)
        {
            if (tab[i][1] <= time)
            {
                readyTasks.push_back(tab[i]);
                tab.erase(tab.begin() + i);
                i--;
                sort = true;
            }
        }

        //sorting readyTasks by p_j
        if (sort)
        {
	        if (mode == "LARGER_FIRST")
	        {
	            if (key2 >= 0)
	            {
                    if (operand == "/")
                    {
                        std::sort(readyTasks.begin(), readyTasks.end(),
                            [&](auto& a, auto& b) { return a[key1] / a[key2] < b[key1] / b[key2]; });
                    }
                    else if (operand != "*")
                    {
                        std::sort(readyTasks.begin(), readyTasks.end(),
                            [&](auto& a, auto& b) { return a[key1] * a[key2] < b[key1] * b[key2]; });
                    }
                    else
                    {
                        cout << "\nUNKNOWN OPERAND\n";
                        exit(0);
                    }
	            }
	            else
	            {
                    std::sort(readyTasks.begin(), readyTasks.end(),
                        [&](auto& a, auto& b) { return a[key1] < b[key1]; });
	            }
	        }
	        else if (mode == "SMALLER_FIRST")
	        {
	            if (key2 >= 0)
	            {
                    if (operand == "/")
                    {
                        std::sort(readyTasks.begin(), readyTasks.end(),
                            [&](auto& a, auto& b) { return a[key1] / a[key2] > b[key1] / b[key2]; });
                    }
                    else if (operand != "*")
                    {
                        std::sort(readyTasks.begin(), readyTasks.end(),
                            [&](auto& a, auto& b) { return a[key1] * a[key2] > b[key1] * b[key2]; });
                    }
                    else
                    {
                        cout << "\nUNKNOWN OPERAND\n";
                        exit(0);
                    }
	            }
	            else
	            {
                    std::sort(readyTasks.begin(), readyTasks.end(),
                        [&](auto& a, auto& b) { return a[key1] > b[key1]; });
	            }
	        }
	        else
	        {
	            cout << "\nUNKNOWN SORTING MODE: '" << mode << "'\n";
	            exit(0);
	        }
	    }

        //moving tasks to ongoingTasks from readyTasks, adding them to greedySolution answer and locking nodes
        for (int i = 0; i < readyTasks.size(); i++)
        {
            if (readyTasks[i][4] <= availableNodes)
            {
                readyTasks[i][2] = time;
                /*for (int j = 0; j < readyTasks[i][4]; j++) //Przydzielanie konkretnych procesorów dla zadania i usuwanie ich z freeProcessors
                        {
                        readyTasks[i].push_back(freeProcessors[0]);
                        freeProcessors.erase(freeProcessors.begin());
                        }*/
                if (convert)
                    for (int j = 0; j < readyTasks[i][4]; j++) //Przydzielanie konkretnych procesorów dla zadania i usuwanie ich z freeProcessors
                    {
                        readyTasks[i].push_back(*(freeProcessors.end() - 1));
                        freeProcessors.pop_back();
                    }
                ongoingTasks.push_back(readyTasks[i]);

                greedySolution.push_back(readyTasks[i]);
                if (convert)
                {
                    greedySolution[greedySolution.size() - 1][3] += greedySolution[greedySolution.size() - 1][2];
                    greedySolution[greedySolution.size() - 1].erase(greedySolution[greedySolution.size() - 1].begin() + 1);
                    greedySolution[greedySolution.size() - 1].erase(greedySolution[greedySolution.size() - 1].begin() + 3);
                }

                importantTimeValues.push_back(readyTasks[i][2] + readyTasks[i][3]);
                availableNodes -= readyTasks[i][4];
                readyTasks.erase(readyTasks.begin() + i);
                i--;
            }
        }
    }
    //QS(greedySolution, 0, greedySolution.size() - 1, 0);
    return greedySolution;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void convertion(vector<vector<int>> &tab)
{
    vector<vector<int>> ongoingTasks;
    vector<int> importantTimeValues;
    vector<int> freeProcessors;
    int freeProcs = maxNodes;
    int time;

    for (int i = 0; i < maxNodes; i++)
    {
        freeProcessors.push_back(i);
    }

    for (int i = 0; i < tab.size(); i++)
        for (int j = i; j < tab.size(); j++)
            if (tab[i][2] < tab[j][2])
                std::swap(tab[i], tab[j]);

    for (int i = 0; i < tab.size(); i++)
    {
        importantTimeValues.push_back(tab[i][2]);
        importantTimeValues.push_back(tab[i][2] + tab[i][3]);
    }

    int i = 0;
    bool flag;
    while (!importantTimeValues.empty() && i < tab.size())
    {
        //cout << "\nFREE PROCS: " << freeProcs << endl;
        flag = true;
        takeSmallestTimeValue(importantTimeValues, time);
        //cout << "\nSIZE:" << importantTimeValues.size() << " TIME:" << time << " i:" << i;

        for (int j = 0; j < ongoingTasks.size(); j++)
            if (ongoingTasks[j][3] == time - ongoingTasks[j][2]) //TASK DURATION == NOW - START TIME - sprawdzenie czy zadanie właśnie się skończyło
            {                                                    //cout << "\nUPUP i: " << i;
                freeProcs += ongoingTasks[j][4];                 //UP: jeśli tak to zwiększamy liczbę dostępnych procesorów, bo zadanie skończyło się wykonywaÄ‡
                for (int k = 0; k < ongoingTasks[j][4]; k++)
                {
                    freeProcessors.push_back(ongoingTasks[j][k + 5]); //Dodajemy do listy wolnych procesorów te zwolnione przez zadanie, które sie zakończyło
                }
                ongoingTasks.erase(ongoingTasks.begin() + j); //i usuwamy to zadanko z ongoing tasks
                flag = false;
                break;
            }

        //cout << "\nUP i: " << i;
        if (time == tab[tab.size() - i - 1][2] && flag && freeProcs >= tab[tab.size() - i - 1][4])
        {
            //cout << "\nDOWN i: " << i << endl;
            for (int j = 0; j < tab[tab.size() - i - 1][4]; j++) //Przydzielanie konkretnych procesorów dla zadania i usuwanie ich z freeProcessors
            {
                //cout << "\nprocs: " << freeProcessors.size() << endl;
                tab[tab.size() - i - 1].push_back(*(freeProcessors.end() - 1));
                freeProcessors.pop_back();
            }
            ongoingTasks.push_back(tab[tab.size() - i - 1]);
            freeProcs -= tab[tab.size() - i - 1][4];
            i++;
        }
        //cout << "\nDN i: " << i << endl;
    }

    for (int i = 0; i < tab.size(); i++)
    {
        tab[i][3] += tab[i][2];
        tab[i].erase(tab[i].begin() + 1);
        tab[i].erase(tab[i].begin() + 3);
    }
}

vector<vector<int>> neighbour(vector<vector<int>> tab, int tasksToSwap)
{
    vector<vector<int>> ongoingTasks;
    vector<vector<int>> neighbourTab;
    vector<int> importantTimeValues;
    int availableNodes = maxNodes;
    int time;

    importTasksAppearanceTimes(tab, importantTimeValues);
    takeSmallestTimeValue(importantTimeValues, time);

    for (int i = 0; i < tasksToSwap; i++)
        doTask(tab.at(i), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    doTask(tab.at(tasksToSwap + 1), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);
    doTask(tab.at(tasksToSwap), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    for (int i = tasksToSwap + 2; neighbourTab.size() != tab.size(); i++)
        doTask(tab.at(i), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    return neighbourTab;
}

vector<vector<int>> neighbour2(vector<vector<int>> tab, int taskToSwap)
{
    vector<vector<int>> ongoingTasks;
    vector<vector<int>> neighbourTab;
    vector<int> importantTimeValues;
    int availableNodes = maxNodes;
    int time;

    importTasksAppearanceTimes(tab, importantTimeValues);
    takeSmallestTimeValue(importantTimeValues, time);

    for (int i = 0; i < taskToSwap; i++)
        doTask(tab.at(i), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    doTask(tab.at(taskToSwap + 2), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);
    doTask(tab.at(taskToSwap + 1), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);
    doTask(tab.at(taskToSwap), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    for (int i = taskToSwap + 3; neighbourTab.size() != tab.size(); i++)
        doTask(tab.at(i), neighbourTab, ongoingTasks, time, availableNodes, importantTimeValues);

    return neighbourTab;
}

vector<vector<int>> heur2(vector<vector<int>> tab, int limit)
{
    double heur2time = clock();
    vector<vector<int>> heur2Solution = greedyTaskDuration(tab, 3, 4, "SMALLER_FIRST", "/", false);
    if (heur2Solution.size() == 1)
    {
        convertion(heur2Solution);
        //cout << "neighbour loops done: " << 0 << endl;
        //cout << "\nTIME\n";
        return heur2Solution;
    }
    vector<vector<int>> newNeighbour;

    //looking for neighbours 2 spots away
    for (int j = 0; j < heur2Solution.size() - 2; j++)
    {
        //cout << "\nERR\n";
        newNeighbour = neighbour2(heur2Solution, j);
        if (durationNotConverted(newNeighbour) < durationNotConverted(heur2Solution))
        {
            //cout << "\nCHANGE\n";
            heur2Solution = newNeighbour;
        }

        if (limit < countTime(clock() - heur2time))
        {
            convertion(heur2Solution);
            cout << "neighbour loops done: " << j << endl;
            //cout << "\nTIME\n";
            return heur2Solution;
        }
        //return {};
    }

    //looking for neighbours 1 spot away
    /*	for (int j = 0; j < heur2Solution.size() - 2; j++)
        {
                vector <vector <int> > newNeighbour = neighbour(heur2Solution, j);
                if (duration(newNeighbour) < duration(heur2Solution))
                        heur2Solution = newNeighbour;

                if (300 < countTime(clock() - heur2time))
                        return {};
        }*/

    convertion(heur2Solution);
    return heur2Solution;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    if (argc != 4)
        {
                cout << "\nZla liczba argumentow. Format: ./[nazwa pliku z programem] [nazwa pliku txt z instancja] [ile zadan ma zostac uszeregowanych] [limit czasu na wykonanie zadania przez algorytm zamieniajacy zadania].\n";
                exit(0);
        }

    string fileName = argv[1];
    int howManyTasks = atoi(argv[2]);
    int timeLimit = atoi(argv[3]);
    double t;

    /*string fileName;
    int howManyTasks, timeLimit;
    cout << "file: ";
    cin >> fileName;
    cout << "\ntasks: ";
    cin >> howManyTasks;
    cout << "\nlimit: ";
    cin >> timeLimit;
    cout << endl;*/

    readTasks(howManyTasks, fileName); //READ TASKS imports data to global vector 'tasks'
    howManyTasks = realHowMany;
    cout << right << setw(20) << "tasks: " << left << setw(7) << howManyTasks << endl;
    //printVector(tasks);

    t = clock();
    vector<vector<int>> greedySolution = greedyTaskDuration(tasks, 3, 4, "SMALLER_FIRST", "/", true); //LARGER_FIRST or SMALLER_FIRST
    t = countTime(clock() - t);
    double durGreedy = duration(greedySolution);
    cout << right << setw(12) << "greedy: " << left << setw(7) << durGreedy << " dur: " << t << " sec" << endl;
    exportSolution(greedySolution, "solution_greedy_" + to_string(howManyTasks) + ".txt");

    t = clock();
    vector<vector<int>> h2Solution = heur2(tasks, timeLimit);
    t = countTime(clock() - t);
    if (!h2Solution.size())
        exit(0);
    double durh2 = duration(h2Solution);
    cout << right << setw(12) << "neighbour: " << left << setw(7) << durh2 << " dur: " << t << " sec" << right << setw(10) << "dif: " << durGreedy - durh2 << " (" << (durGreedy - durh2) / durGreedy * 100 << "%)" << endl
         << endl;
    exportSolution(h2Solution, "solution_h2_" + to_string(howManyTasks) + ".txt");

    tasks.clear();

    cout << "Koniec" << endl;
    return 0;
}
