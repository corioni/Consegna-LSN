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

void uniform_integral(int N_throws, int N_blocks, string file_name, Random &rnd, const Function &f)
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

void RW_discrete(int N_walk, int N_step, int N_blocks, double a, std::string file_name, Random &rnd)
{

  double sum = 0.;
  double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
  double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
  double avg = 0.;                // Variable to hold block averages
  double err = 0.;                // Variable to hold uncertainties

  vector<vector<int>> x(N_walk, vector<int>(3, 0)); // x,y,z

  ofstream out(file_name);
  out << "comulative_averages\t uncertainty" << endl;
  if (!out)
  {
    cout << "error opening output file!" << endl;
  }
  err = uncertainty_estimation(cumulative_sum / (N_blocks), cumulative_sum_sq / (N_blocks), N_blocks);
  out << (cumulative_sum / (N_blocks)) << "\t" << err << endl;
  for (int k = 1; k < N_step; k++)
  {
    cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
    cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
    avg = 0.;                // Variable to hold block averages
    err = 0.;                // Variable to hold uncertainties
    for (int j = 0; j < N_blocks; j++)
    {
      sum = 0.0;
      for (int i = 0; i < (N_walk / N_blocks); i++)
      {
        x[i + j * (N_walk / N_blocks)][discrete_sample(rnd, 0, 2)] += sign_sample(rnd) * a;
        sum += square_distance(x[i + j * (N_walk / N_blocks)]);
      }
      avg = sqrt(sum / (N_walk / N_blocks));
      cumulative_sum += avg;
      cumulative_sum_sq += avg * avg;
    }
    err = uncertainty_estimation(cumulative_sum / (N_blocks), cumulative_sum_sq / (N_blocks), N_blocks);
    out << (cumulative_sum / (N_blocks)) << "\t" << err << endl;
  }
  out.close();
}

void RW_continuous(int N_walk, int N_step, int N_blocks, double a, string file_name, Random &rnd)
{

  double sum = 0.;
  double cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
  double cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
  double avg = 0.;                // Variable to hold block averages
  double err = 0.;                // Variable to hold uncertainties

  vector<vector<double>> x(N_walk, vector<double>(3, 0)); // x,y,z

  ofstream out(file_name);
  out << "comulative_averages\t uncertainty" << endl;
  if (!out)
  {
    cout << "error opening output file!" << endl;
  }
  err = uncertainty_estimation(cumulative_sum / (N_blocks), cumulative_sum_sq / (N_blocks), N_blocks);
  out << (cumulative_sum / (N_blocks)) << "\t" << err << endl;
  for (int k = 1; k < N_step; k++)
  {
    cumulative_sum = 0.0;    // Variable to hold cumulative sum of averages
    cumulative_sum_sq = 0.0; // Variable to hold cumulative sum of squared averages
    avg = 0.;                // Variable to hold block averages
    err = 0.;                // Variable to hold uncertainties
    for (int j = 0; j < N_blocks; j++)
    {
      sum = 0.0;
      for (int i = 0; i < (N_walk / N_blocks); i++)
      {
        x[i + j * (N_walk / N_blocks)][0] += coutinue_sample(rnd)[0] * a;
        x[i + j * (N_walk / N_blocks)][1] += coutinue_sample(rnd)[1] * a;
        x[i + j * (N_walk / N_blocks)][2] += coutinue_sample(rnd)[2] * a;
        sum += square_distance(x[i + j * (N_walk / N_blocks)]);
      }
      avg = sqrt(sum / (N_walk / N_blocks));
      cumulative_sum += avg;
      cumulative_sum_sq += avg * avg;
    }
    err = uncertainty_estimation(cumulative_sum / (N_blocks), cumulative_sum_sq / (N_blocks), N_blocks);
    out << (cumulative_sum / (N_blocks)) << "\t" << err << endl;
  }
  out.close();
}

int discrete_sample(Random &rnd, int n_min, int n_max)
{
  double x = rnd.Rannyu();
  int N = (n_max - n_min) + 1;
  for (int i = 0; i < N; i++)
  {
    if (x > (double)i / N && x <= (double)(i + 1) / N)
      return (n_min + i);
  }
}
int sign_sample(Random &rnd)
{
  double x = rnd.Rannyu(0.,1.);
  if (x >= 0.5)
    return 1;
  else
    return -1;
}

vector<double> coutinue_sample(Random &rnd)
{
  double theta, phi;
  theta = rnd.Rannyu(0, M_PI);
  phi = rnd.Rannyu(0, 2 * M_PI);

  return {sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)};
}

template <typename T>
double square_distance(vector<T> A)
{
  double x = 0.;
  for (int i = 0; i < A.size(); i++)
    x += pow(A[i], 2.);

  return x;
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
