#include "/home/marta/LSN/Lab1/es_01.3/random/random.h"
#include "/home/marta/LSN/Lab1/es_01.3/utilities/utilities.h"
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

    // setting parameters
    double d = 1.;  // distance between lines
    double l = 0.8; // length of the needle
    int N_hits = 0; // Number of hits

    int Nthrows = 1e6; // Number of throws per block
    int Nblocks = 100;        // number of blocks

    double pi = 0.;
    double err = 0.;

    double cumulative_sum = 0.;
    double cumulative_sum_sq = 0.;

    ofstream out("output/es01_3.dat");
    out << "comulative_averages\t uncertainty" << endl;
    if (!out)
    {
        cout << "Errore apertura file!\n";
        return -1;
    }

    for (int i = 0; i < Nblocks; i++)
    {
        N_hits = 0.;
        for (int j = 0; j < Nthrows; j++)
        {
            if (needle_toss(l,d, rnd)) // Generate a random number between 0 and 1
                N_hits++;
        }
        pi = 2 * l * (double(Nthrows)) / (double(N_hits) * d);
        cumulative_sum += pi;
        cumulative_sum_sq += pi * pi;
        err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
        out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
    }
    out.close();

    rnd.SaveSeed();
    return 0;
}