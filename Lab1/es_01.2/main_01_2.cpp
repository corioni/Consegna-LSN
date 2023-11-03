#include "/home/marta/LSN/Lab1/es_01.2/random/random.h"
#include "/home/marta/LSN/Lab1/es_01.2/utilities/utilities.h"
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
    const int Nthrows = 1e4;

    // 1)
    ofstream out("output/es01_2_1.dat");
    out << "uniforme\texponenziale\tlorentziana" << endl;
    for (int i = 0; i < Nthrows; i++)
    {
        out << rnd.Rannyu() << "\t" << rnd.Exp(1.) << "\t" << rnd.Lorentz(1., 0.) << endl;
    }
    out.close();

    // 2)
    int N[4] = {1, 2, 10, 100};
    double sum_unif = 0.0;
    double sum_expo = 0.0;
    double sum_lore = 0.0;

    ofstream out_u("output/es01_2_2a.dat");
    ofstream out_e("output/es01_2_2b.dat");
    ofstream out_l("output/es01_2_2c.dat");

    out_u << "N=1\tN=2\tN=10\tN=100" << endl;
    out_e << "N=1\tN=2\tN=10\tN=100" << endl;
    out_l << "N=1\tN=2\tN=10\tN=100" << endl;

    for (int i = 0; i < Nthrows; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            sum_unif = 0.0;
            sum_expo = 0.0;
            sum_lore = 0.0;
            for (int n = 0; n < N[j]; n++)
            {
                sum_unif += rnd.Rannyu();
                sum_expo += rnd.Exp(1.);
                sum_lore += rnd.Lorentz(1., 0.);
            }
            out_u << sum_unif / N[j] << "\t";
            out_e << sum_expo / N[j] << "\t";
            out_l << sum_lore / N[j] << "\t";
        }
        out_u << endl;
        out_e << endl;
        out_l << endl;
    }
    out_u.close();
    out_e.close();
    out_l.close();
    // Save the random number generator seed for reproducibility
    rnd.SaveSeed();
    return 0;
}