#include "./random/random.h"
#include "./utilities/utilities.h"
#include "./utilities/option.h"
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

    int N_throws = int(1e4); // number of throws per block M
    int N_blocks = 100;      // number of blocks N

    double S0 = 100.;
    double K = 100.;
    double T = 1.;
    double r = 0.1;
    double sigma = 0.25;
    // double t = 0 ;

    CallOption call(S0, K, r, T, sigma);
    PutOption put(S0, K, r, T, sigma);

    direct_cumulative_average(N_throws, N_blocks, "output/es03_1c.dat", rnd, call);
    direct_cumulative_average(N_throws, N_blocks, "output/es03_1p.dat", rnd, put);
    discretized_cumulative_average(N_throws, N_blocks, "output/es03_2c.dat", rnd, call);
    discretized_cumulative_average(N_throws, N_blocks, "output/es03_2p.dat", rnd, put);

    rnd.SaveSeed();
    return 0;
}