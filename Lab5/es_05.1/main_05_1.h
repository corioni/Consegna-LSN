#ifndef __main_05_01_h__
#define __main_05_01_h__

#include "function.h"
#include "random.h"


Random rnd;

// userful variables
int N_block, N_step;
bool sample_u=true; // do we want a uniform or gaussian sampling?
double step=1.;
int accepted;
const int wd = 20;

// initial position
std::vector<double> x0={0.,0.,0.};

// functions
void Initialize_Random();
double uncertainty_estimation(double, double, int);
double square_mod(std::vector<double> x);
void integral(std::string file_name,std::string file_name2, const Function &);
std::vector<double> Metropolis_step(std::vector<double> x,const Function &psi);
void perform_integrals(const std::string output_file_prefix, const Function &psi, double step_u, double step_g);



#endif