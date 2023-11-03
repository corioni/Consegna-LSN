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

    Initialize_Random(rnd);

    int N_throws = 10000; // number of samples
    int N_blocks = 100;   // number of blocks M

    // initialize integrand function in function.h
    MyFunction f;

    // 1)
    // evaluate and print integral in blocks
    uniform_integral(N_throws, N_blocks, "output/es02_1u.dat", rnd, f);

    // 2)

    // define normalized 2 distributions
    Straight_Line l(-2., 2.);
    Parable p(-3. / 2., 0, 3. / 2.);
    Exponential e(1.);

    // evaluate and print importamce integral in blocks
    importance_integral_AR(N_throws, N_blocks, "output/es02_1l.dat", rnd, f, l);
    importance_integral_AR(N_throws, N_blocks, "output/es02_1p.dat", rnd, f, p);
    importance_integral_exp(N_throws, N_blocks, "output/es02_1e.dat", rnd, f, e);

    rnd.SaveSeed();
    return 0;
}