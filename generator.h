#pragma once

int ceiling(int nr, int div);

void generateInstance(int procsLow = 3,
                      int procsHigh = 10000,
                      int tasksLow = 2,
                      int tasksHigh = 200,
                      int optimalFactor = 1000,
                      bool nrOfTasksManually = false);
