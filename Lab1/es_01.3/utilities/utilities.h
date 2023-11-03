#ifndef __utilities_h__
#define __utilities_h__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include "/home/marta/LSN/Lab1/es_01.3/random/random.h"

void Initialize_Random(Random&);
double uncertainty_estimation(double, double, int);
bool needle_toss(double l, double d, Random &rnd);


#endif