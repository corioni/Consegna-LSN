#include "main_08_2.h"
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


    N_fit = 5000;

    // define file names for the output
    file_DB = "output/energy_DB.dat";
    file_params = "output/parameters.dat";
    file_histo = "output/histo_psi.dat";

    initial_temperature = 0.1; // Initial temperature
    cooling_rate = 0.995; // Cooling rate

    // Starting parameters
    double mu = 0.80;
    double sigma = 0.60;
    //  Define initial position
    x0 = 0.0;

    // finding the best parameters
    cout << "Finding the best parameters to minimize energy." << endl;
    vector<double> opt = fit_parameters(mu, sigma);
    cout << "The best parameters are:\t"
         << "mu = " << opt[0] << "\tsigma = " << opt[1] << endl;

    // Parameters for data blocking
    N_block = 100;
    N_step = 10000;

    mu = opt[0];
    sigma = opt[1];
    data_blocking_print(mu, sigma); // I also print the DB for the best fit

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

double data_blocking(double mu, double sigma)
{

    double x = x0;
    double cumulative_sum = 0.0; // Variable to hold cumulative sum of averages

    for (int i = 0; i <= N_block; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < N_step; j++)
        {
            x = Metropolis_step(x, mu, sigma);
            sum += f.calculate(x, mu, sigma);
        }
        if (i > 0)
        { // skippiamo il primo blocco per equilibrare il sistema
            double avg = sum / N_step;
            cumulative_sum += avg;
        }
    }
    return cumulative_sum / N_block;
}

double data_blocking_print(double mu, double sigma)
{

    double x = x0;
    double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
    double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages

    ofstream out_ave(file_DB);
    out_ave << setw(wd) << "block" << setw(wd) << "cumulative_averages" << setw(wd) << "uncertainty" << endl;

    ofstream out_x(file_histo);

    if (!out_ave)
    {
        cout << "Error opening output file!" << endl;
        return -1;
    }
    if (!out_x)
    {
        cout << "Error opening output file!" << endl;
        return -1;
    }

    accepted = 0;
    for (int i = 0; i <= N_block; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < N_step; j++)
        {
            x = Metropolis_step(x, mu, sigma);
            sum += f.calculate(x, mu, sigma);
            out_x << x << endl;
        }
        if (i > 0)
        { // skippiamo il primo blocco per equilibrare il sistema
            double avg = sum / N_step;
            cumulative_sum += avg;
            cumulative_sum_sq += avg * avg;
            double err = uncertainty_estimation(cumulative_sum / i, cumulative_sum_sq / i, i - 1);
            out_ave << setw(wd) << i << setw(wd) << cumulative_sum / i << setw(wd) << err << endl;
        }
    }
    cout << "acceptance probability  ->" << (accepted / ((double)(N_block + 1) * N_step)) * 100 << " %" << endl;
    out_ave.close();
    out_x.close();

    return cumulative_sum / N_block;
}

double Metropolis_step(double x, double mu, double sigma)
{

    double new_x;

    for (int i = 0; i < 3; i++)
    {
        new_x = rnd.Rannyu(x - step, x + step);
        // se non è uniforme è gauss
    }

    double alpha = min(1.0, (psi.calculate(new_x, mu, sigma)*psi.calculate(new_x, mu, sigma)) / (psi.calculate(x, mu, sigma)*psi.calculate(x, mu, sigma)));

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

// Function to print a progress bar
void printProgressBar(double progress)
{
    const int barWidth = 70;
    int pos = barWidth * progress;
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

std::vector<double> fit_parameters(double initial_mu, double initial_sigma)
{
    const double parameter_change_range = 0.01;
    N_block = 10;
    N_step = 10000;

    double mu = initial_mu;
    double sigma = initial_sigma;
    double energy_old, energy_new;
    double T = initial_temperature;

    // Define the file_params variable
    std::ofstream out(file_params);

    energy_old = data_blocking(mu, sigma);

    for (int i = 0; i < N_fit; i++)
    {
        // Generate small random changes in parameters
        double delta_mu = parameter_change_range * (2.0 * rnd.Rannyu() - 1.0);
        double delta_sigma = parameter_change_range * (2.0 * rnd.Rannyu() - 1.0);

        // Propose new parameters
        double new_mu = mu + delta_mu;
        double new_sigma = sigma + delta_sigma;

        // Calculate the energy with the new parameters
        energy_new = data_blocking(new_mu, new_sigma);

        // Metropolis acceptance criterion
        if (energy_new < energy_old || rnd.Rannyu() < exp((energy_old - energy_new) / T))
        {
            mu = new_mu;
            sigma = new_sigma;
            energy_old = energy_new;
        }

        // Write the updated parameters and energy to the output file
        out << std::setw(12) << mu << std::setw(12) << sigma << std::setw(12) << energy_new << std::endl;

        // Calculate and print the progress bar
        double progress = static_cast<double>(i) / static_cast<double>(N_fit);
        printProgressBar(progress);

        // Reduce temperature (cooling)
        T *= cooling_rate;
    }

    out.close();
    std::cout << std::endl; // Print a newline to clear the progress bar

    return {mu, sigma};
}


