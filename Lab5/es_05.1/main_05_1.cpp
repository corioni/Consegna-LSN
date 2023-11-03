#include "main_05_1.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <numeric>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main()
{

    // Initialize Random number generator
    Initialize_Random();

    // userful variables
    N_block = 100;
    N_step = 100000;

     double z[2]={1.,20,}; //initial z for the 2 cases

     for (int i = 1; i <=2; i++)
    {
    //  Define initial position
    x0 = {0., 0.,  z[i-1]};

    // 1) Hydrogen Atom 100
    Hydrogen_Atom_100 psi_100;
    perform_integrals("output"+to_string(i)+"/psi100_", psi_100, 1.2, 0.7);//we tried different values of the step to reach the 50% empirical rule

    // 2) Hydrogen Atom 210
    Hydrogen_Atom_210 psi_210;
    perform_integrals("output"+to_string(i)+"/psi210_", psi_210, 2.9, 1.9);//we tried different values of the step to reach the 50% empirical rule
    }
    rnd.SaveSeed();
    return 0;
}

void Initialize_Random()
{
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open())
    {
        Primes >> p1 >> p2;
    }
    else
        cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open())
    {
        while (!input.eof())
        {
            input >> property;
            if (property == "RANDOMSEED")
            {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    }
    else
        cerr << "PROBLEM: Unable to open seed.in" << endl;
}

double uncertainty_estimation(double avg, double sq_avg, int n)
{
    if (n == 0)
        return 0;
    else
        return sqrt((sq_avg - avg * avg) / n);
}

double square_mod(vector<double> x)
{
    return sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}

void integral(string file_name, string file_name2, const Function &f)
{

    vector<double> x = x0;
    double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
    double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages

    double cumulative_avg = 0.0;    
    double cumulative_avg_sq = 0.0; 
    double err=0;


    ofstream out_ave(file_name);
    out_ave << setw(wd) << "block" << setw(wd) << "cumulative_averages" << setw(wd) << "uncertainty" << endl;

    ofstream out_x(file_name2);
    out_x << setw(wd) << "x" << setw(wd) << "y" << setw(wd) << "z" << endl;

    if (!out_ave)  cout << "Error opening output file!" << endl;
    if (!out_x)    cout << "Error opening output file!" << endl;
    
    accepted = 0;
    for (int i = 0; i <= N_block; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < N_step; j++)
        {
            x = Metropolis_step(x, f);
            sum += square_mod(x);
            if(j%1000==0)
                out_x << setw(wd) << x[0] << setw(wd) << x[1] << setw(wd) << x[2] << endl;
        }
        if(i>0){ //skippiamo il primo per equilibrare il sistema
        double avg = sum / N_step;
        cumulative_sum += avg;
        cumulative_sum_sq += avg * avg;

        cumulative_avg = cumulative_sum / i;
        cumulative_avg_sq = cumulative_sum_sq / i;
        err = uncertainty_estimation(cumulative_avg, cumulative_avg_sq, i - 1);}
        out_ave << setw(wd) << i << setw(wd) << cumulative_avg << setw(wd) << err << endl;
    }
    cout << "acceptance probability (" << file_name << ") ->" << (accepted / ((double)(N_block+1) * N_step)) * 100 << " %" << endl;
    out_ave.close();
    out_x.close();
}

vector<double> Metropolis_step(vector<double> x, const Function &psi)
{

    vector<double> new_x(3, 0.);

    for (int i = 0; i < 3; i++)
    {
        new_x[i] = (sample_u) ? rnd.Rannyu(x[i] - step, x[i] + step) : rnd.Gauss(x[i], step);
        // se non è uniforme è gauss
    }

    double alpha = min(1.0, psi.calculate(new_x) / psi.calculate(x));

    if (rnd.Rannyu(0.0, 1.0) < alpha)
    {
        accepted++;
        return new_x;
    }
    else
    {
        return x;
    }
}

void perform_integrals(const string output_file_prefix, const Function &psi, double step_u, double step_g)
{
    sample_u = true; // Uniform sampling
    step = (sample_u) ? step_u : step_g;
    integral(output_file_prefix + "ave_u.dat", output_file_prefix + "x_u.dat", psi);

    sample_u = false; // Gaussian sampling
    step = (sample_u) ? step_u : step_g;
    integral(output_file_prefix + "ave_g.dat", output_file_prefix + "x_g.dat", psi);
};
