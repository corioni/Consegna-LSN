#include "utilities.h"

using namespace std;

void Initialize_Random(Random &rnd)
{

  int seed[4];
  int p1, p2;
  ifstream Primes("/home/marta/LSN/Lab1/es_01.2/random/Primes");
  if (Primes.is_open())
  {
    Primes >> p1 >> p2;
  }
  else
    cerr << "PROBLEM: Unable to open Primes" << endl;
  Primes.close();

  ifstream input("/home/marta/LSN/Lab1/es_01.2/random/seed.in");
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


double uncertainty_estimation(double avg, double sq_avg, int n){
    if (n==0)
        return 0;
    else
        return sqrt((sq_avg - avg*avg)/n);
}

