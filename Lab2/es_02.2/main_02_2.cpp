#include "./random/random.h"
#include "./utilities/utilities.h"
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

    int N_walk = 10000; // number of samples
    int N_blocks = 100;   // number of blocks M
    int N_step = 100;
    int step_lentgh = 1.;

    RW_discrete(N_walk,  N_step,  N_blocks, step_lentgh, "output/es02_2d.dat", rnd);
    RW_continuous(N_walk,  N_step,  N_blocks, step_lentgh, "output/es02_2c.dat", rnd);



    rnd.SaveSeed();
    return 0;
}