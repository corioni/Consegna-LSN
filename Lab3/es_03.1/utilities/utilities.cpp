#include "utilities.h"

using namespace std;

void Initialize_Random(Random &rnd)
{

  int seed[4];
  int p1, p2;
  ifstream Primes("./random/Primes");
  if (Primes.is_open())
  {
    Primes >> p1 >> p2;
  }
  else
    cerr << "PROBLEM: Unable to open Primes" << endl;
  Primes.close();

  ifstream input("./random/seed.in");
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


void direct_cumulative_average(int N_throws, int N_blocks, string file_name, Random &rnd,const  EuropeanOption &opt)
{

  double sum = 0.;
  double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
  double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
  double avg = 0.;                // Variable to hold block averages
  double err = 0.;                // Variable to hold uncertainties

  ofstream out(file_name);
  out << "comulative_averages\t uncertainty" << endl;
  if (!out)
  {
    cout << "error opening output file!" << endl;
  }

  for (int i = 0; i < N_blocks; i++)
  {
    sum = 0.0;
    for (int j = 0; j < N_throws; j++)
    {
      sum += opt.direct_simulation(rnd);
    }
    avg = sum / N_throws;
    cumulative_sum += avg;
    cumulative_sum_sq += avg * avg;
    err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
    out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
  }
  out.close();
}

void discretized_cumulative_average(int N_throws, int N_blocks, string file_name, Random &rnd,const  EuropeanOption &opt)
{

  double sum = 0.;
  double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
  double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
  double avg = 0.;                // Variable to hold block averages
  double err = 0.;                // Variable to hold uncertainties

  ofstream out(file_name);
  out << "comulative_averages\t uncertainty" << endl;
  if (!out)
  {
    cout << "error opening output file!" << endl;
  }

  for (int i = 0; i < N_blocks; i++)
  {
    sum = 0.0;
    for (int j = 0; j < N_throws; j++)
    {
      sum += opt.discretized_simulation(rnd);
    }
    avg = sum / N_throws;
    cumulative_sum += avg;
    cumulative_sum_sq += avg * avg;
    err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
    out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
  }
  out.close();
}