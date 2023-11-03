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
#include "../random/random.h"

void Initialize_Random(Random&);
double uncertainty_estimation(double, double, int);
bool needle_toss(double l, double d, Random &rnd);
void uniform_integral(int N_throws, int N_blocks,std::string file_name,Random &rnd, const Function&);
void importance_integral_AR(int N_throws, int N_blocks, string file_name, Random &rnd, const Function &f, const Function& p);
void importance_integral_exp(int N_throws, int N_blocks, string file_name, Random &rnd, const Function &f, const Exponential &e);
double goldenSectionSearch(const Function &func, double x_min, double x_max, double epsilon);


void RW_discrete(int N_walk, int N_step, int N_blocks, double  a, std::string file_name, Random &rnd);
void RW_continuous(int N_walk, int N_step, int N_blocks, double  a, std::string file_name, Random &rnd);
int discrete_sample(Random &rnd, int n_min, int n_max);
int sign_sample(Random &rnd);
vector<double> coutinue_sample(Random &rnd);
template <typename T> double square_distance(vector<T> A);


#endif