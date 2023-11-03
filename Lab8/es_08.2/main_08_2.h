#ifndef __main_08_02_h__
#define __main_08_02_h__

#include "function.h"
#include "random.h"


Random rnd;

// userful variables
int N_block, N_step, N_fit;
double step=2.7;
int accepted;
const int wd = 20;

Wave_Function psi;  // function for metropolis step
Integrand f;        // integrand

std::string file_DB;
std::string file_params;
std::string file_histo;

double initial_temperature ; // Initial temperature
double cooling_rate; // Cooling rate
double temp;


// initial position
double x0 =0.;

// functions
void Initialize_Random();
double uncertainty_estimation(double, double, int);
double data_blocking(double mu, double sigma );// fist function is the integrand, the second is used to sample the metropolis step
double data_blocking_print(double mu, double sigma );// fist function is the integrand, the second is used to sample the metropolis step
double Metropolis_step(double x,double mu,double sigma);
vector<double> fit_parameters(double mu, double sigma);
void printProgressBar(double progress);


#endif