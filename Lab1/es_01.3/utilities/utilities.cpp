#include "utilities.h"

using namespace std;

void Initialize_Random(Random &rnd)
{

  int seed[4];
  int p1, p2;
  ifstream Primes("/home/marta/LSN/Lab1/es_01.3/random/Primes");
  if (Primes.is_open())
  {
    Primes >> p1 >> p2;
  }
  else
    cerr << "PROBLEM: Unable to open Primes" << endl;
  Primes.close();

  ifstream input("/home/marta/LSN/Lab1/es_01.3/random/seed.in");
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



bool needle_toss(double l, double d, Random &rnd)
{
  double y1 = rnd.Rannyu(0., d); // y coordinate of the first end
  double u = 0., v = 0.;
  // generate the first coordinate of the second end
  do
  {
    // the needle orientation is generated through an accept/reject method in order not to use pi and trigonometric functions
    u = rnd.Rannyu(-1., 1.);
    v = rnd.Rannyu(0., 1.);

    // reject the orientation vector if it doesn't fall in the unitary (quarter-)circle
  } while (u * u + v * v > 1.);
  double y2 = y1 + l * v / sqrt(u * u + v * v);

  if (((y2 > d) || (y2 < 0))) // mi basta una griglia con due righe verticali
    return true;
  else
    return false;
}