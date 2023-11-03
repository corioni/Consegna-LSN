
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "../utilities/function.h"
#include "random.h"

using namespace std;

Random ::Random() {}
// Default constructor, does not perform any action

Random ::~Random() {}
// Default destructor, does not perform any action

void Random ::SaveSeed()
{
   // This function saves the current state of the random number generator to a file "seed.out"
   ofstream WriteSeed;
   WriteSeed.open("/home/marta/LSN/Lab3/es_03.1/random/seed.out");
   if (WriteSeed.is_open())
   {
      WriteSeed << "RANDOMSEED	" << l1 << " " << l2 << " " << l3 << " " << l4 << endl;
      ;
   }
   else
      cerr << "PROBLEM: Unable to open random.out" << endl;
   WriteSeed.close();
   return;
}

double Random ::Gauss(double mean, double sigma)
{
   // This function generates a random number from a Gaussian distribution with given mean and sigma
   double s = Rannyu();
   double t = Rannyu();
   double x = sqrt(-2. * log(1. - s)) * cos(2. * M_PI * t);
   return mean + x * sigma;
}

// Method to generate a random number with a exponential distribution
double Random ::Exp(double lambda)
{
   double y = Rannyu();
   return -1 / lambda * log(1 - y);
}

// Method to generate a random number with a Lorentzian distribution
double Random ::Lorentz(double gamma, double mu)
{
   double y = Rannyu();
   return gamma * tan(M_PI * (y - 0.5)) + mu;
}

double Random ::Rannyu(double min, double max)
{
   // This function generates a random number in the range [min, max)
   return min + (max - min) * Rannyu();
}

double Random ::Rannyu(void)
{
   // This function generates a random number in the range [0,1)
   const double twom12 = 0.000244140625;
   int i1, i2, i3, i4;
   double r;

   i1 = l1 * m4 + l2 * m3 + l3 * m2 + l4 * m1 + n1;
   i2 = l2 * m4 + l3 * m3 + l4 * m2 + n2;
   i3 = l3 * m4 + l4 * m3 + n3;
   i4 = l4 * m4 + n4;
   l4 = i4 % 4096;
   i3 = i3 + i4 / 4096;
   l3 = i3 % 4096;
   i2 = i2 + i3 / 4096;
   l2 = i2 % 4096;
   l1 = (i1 + i2 / 4096) % 4096;
   r = twom12 * (l1 + twom12 * (l2 + twom12 * (l3 + twom12 * (l4))));

   return r;
}

void Random ::SetRandom(int *s, int p1, int p2)
{
   // This function sets the seed and parameters of the random number generator
   m1 = 502;
   m2 = 1521;
   m3 = 4071;
   m4 = 2107;
   l1 = s[0];
   l2 = s[1];
   l3 = s[2];
   l4 = s[3];
   n1 = 0;
   n2 = 0;
   n3 = p1;
   n4 = p2;

   return;
}

double Random::AcceptReject(double x_min, double x_max, double p_max,const Function& f)
{
   double x = 0., y = 0.;
   do
   {
      x = Rannyu(x_min,x_max);
      y = Rannyu(0,p_max);
   } while (y > f.calculate(x) / p_max);
   return x;
}
