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

void uniform_integral(int N_throws, int N_blocks, string file_name, Random &rnd,const Function & f)
{

  double sum = 0.;
  double x = 0.;
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
      x = rnd.Rannyu(); // Generate a random number between 0 and 1
      sum += f.calculate(x);
    }
    avg = sum / N_throws;
    cumulative_sum += avg;
    cumulative_sum_sq += avg * avg;
    err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
    out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
  }
  out.close();
}

void importance_integral_AR(int N_throws, int N_blocks, string file_name, Random &rnd, const Function &f, const Function& p)
{
  double x_min = 0., x_max = 1.;
  double p_max = p.calculate(goldenSectionSearch(p, x_min, x_max, 0.1))+0.1;
  double sum = 0.;
  double x = 0.;
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
      x = rnd.AcceptReject(x_min, x_max, p_max, p); // da modificare p_max
      sum += f.calculate(x)/p.calculate(x);
    }
    avg = sum / N_throws;
    cumulative_sum += avg;
    cumulative_sum_sq += avg * avg;
    err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
    out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
  }
  out.close();
}

void importance_integral_exp(int N_throws, int N_blocks, string file_name, Random &rnd, const Function &f, const Exponential &e)
{
  double x_max = 1.;
  double sum = 0.;
  double x = 0.;
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
      do{
      x = rnd.Exp(e.get_lambda()); 
      } while (x>x_max);
      sum += f.calculate(x)/(e.calculate(x)/(1.-1./M_E));
    }
    avg = sum / N_throws;
    cumulative_sum += avg;
    cumulative_sum_sq += avg * avg;
    err = uncertainty_estimation(cumulative_sum / (i + 1), cumulative_sum_sq / (i + 1), i);
    out << (cumulative_sum / (i + 1)) << "\t" << err << endl;
  }
  out.close();
}

double goldenSectionSearch(const Function &func, double x_min, double x_max, double epsilon)
{
  const double phi = (1 + std::sqrt(5)) / 2; // Golden ratio

  double x1 = x_max - (x_max - x_min) / phi;
  double x2 = x_min + (x_max - x_min) / phi;

  double f1 = func.calculate(x1);
  double f2 = func.calculate(x2);

  while (std::abs(x_max - x_min) > epsilon)
  {
    if (f1 < f2)
    {
      x_min = x1;
      x1 = x2;
      f1 = f2;
      x2 = x_max - (x_max - x_min) / phi;
      f2 = func.calculate(x2);
    }
    else
    {
      x_max = x2;
      x2 = x1;
      f2 = f1;
      x1 = x_min + (x_max - x_min) / phi;
      f1 = func.calculate(x1);
    }
  }

  return (x_max + x_min) / 2;
}


