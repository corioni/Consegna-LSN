#ifndef __utilities_h__
#define __utilities_h__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include "function.h"
#include "option.h"
#include "../random/random.h"

void Initialize_Random(Random &);
double uncertainty_estimation(double, double, int);
void direct_cumulative_average(int N_throws, int N_blocks, string file_name, Random &rnd,const EuropeanOption &opt);
void discretized_cumulative_average(int N_throws, int N_blocks, string file_name, Random &rnd,const EuropeanOption &opt);

#endif