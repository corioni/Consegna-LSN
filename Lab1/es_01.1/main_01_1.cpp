#include "/home/marta/LSN/Lab1/es_01.1/random/random.h"
#include "/home/marta/LSN/Lab1/es_01.1/utilities/utilities.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    // Initialize Random number generator
    Random rnd;
    Initialize_Random(rnd);

    // Initialize constants and variables
    const int Nthrows = 10000; // Number of throws in each block
    const int Nblocks = 100;   // Number of blocks
    double x = 0;              // Variable to hold random numbers
    double avg = 0.;           // Variable to hold block averages
    double err = 0.;           // Variable to hold uncertainties
    double sum = 0.0;          // Variable to hold running sum of random numbers

    double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
    double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages

    // (1) Generate random numbers and calculate averages for each block
    ofstream out1("output/es01_1_1.dat");
    out1 << "comulative_averages\t uncertainty" << endl;
    for (int i = 0; i < Nblocks; i++)
    {
        sum = 0.0;
        for (int j = 0; j < Nthrows; j++)
        {
            x = rnd.Rannyu(); // Generate a random number between 0 and 1
            sum += x;
        }
        avg = sum / Nthrows;
        cumulative_sum += avg;
        cumulative_sum_sq += avg * avg;
        err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
        out1 << (cumulative_sum / (i + 1)) << "\t" << err << endl;
    }
    out1.close();

    // (2) Generate random numbers following function pow((x - 0.5), 2) and calculate averages for each block
    ofstream out2("output/es01_1_2.dat");
    out2 << "comulative_averages\t uncertainty" << endl;

    cumulative_sum = 0.0;
    cumulative_sum_sq = 0.0;
    for (int i = 0; i < Nblocks; i++)
    {
        sum = 0.0;
        for (int j = 0; j < Nthrows; j++)
        {
            x = rnd.Rannyu(); // Generate a random number between 0 and 1
            sum += pow((x - 0.5), 2);
        }
        avg = sum / Nthrows;
        cumulative_sum += avg;
        cumulative_sum_sq += avg * avg;
        err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
        out2 << cumulative_sum / (i + 1) << "\t" << err << endl;
    }
    out2.close();

    // (3) Perform a chi-squared test to analyze the distribution of random numbers
    ofstream out3("output/es01_1_3.dat");
    out3 << "Chi" << endl;

    int Nexec = 100;
    int Exp = Nthrows / Nblocks;
    double Chi = 0.0;
    vector<int> Obs(Nblocks, 0); // Vector to hold observed counts in each block

    for (int i = 0; i < Nexec; i++)
    {
        fill(Obs.begin(), Obs.end(), 0.);
        // Generate random numbers and record their distribution in blocks
        for (int j = 0; j < Nthrows; j++)
        {
            x = rnd.Rannyu();
            Obs[int(x * Nblocks)]++; // Increment count in appropriate block
        }
        Chi = 0.0;
        // Expected count in each block
        for (int j = 0; j < Nblocks; j++)
        {
            Chi += double((Obs[j] - Exp) * (Obs[j] - Exp)) / Exp; // Calculate Chi-squared statistic
        }
        out3 << Chi << endl;
    }
    out3.close();
    // Save the random number generator seed for reproducibility
    rnd.SaveSeed();
    return 0;
}
