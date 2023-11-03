/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "Monte_Carlo_ISING_1D.h"

using namespace std;

int main()
{

  int N_simulations = 15;
  double T_min = 0.5, T_max = 3;

  for (int i = 1; i <= N_simulations; i++)
  {

    // equilibration
    restart = 0;
    Input("input.dat");
    // Inizialization
    temp = T_min + (T_max - T_min) / N_simulations * i;
    beta = 1. / temp;

    for (int iblk = 1; iblk <= nblk; ++iblk) // Simulation
    {
      Reset(iblk); // Reset block averages
      for (int istep = 1; istep <= th_step; ++istep)
        Move(metro);
    }
    ConfFinal(); // Write final configuration

    // simulation

    restart = 1;
    Input("input.dat");
    for (int iblk = 1; iblk <= nblk; ++iblk) // Simulation
    {
      Reset(iblk); // Reset block averages
      for (int istep = 1; istep <= nstep; ++istep)
      {
        Move(metro);
        Measure();
        Accumulate(); // Update block averages
      }
      Averages(iblk); // Print results for current block
    }
    ConfFinal(); // Write final configuration
  }
  return 0;
}

void Input(string file_in)
{
  ifstream ReadInput, Readconf, Seed;

  cout << "Classic 1D Ising model             " << endl;
  cout << "Monte Carlo simulation             " << endl
       << endl;
  cout << "Nearest neighbour interaction      " << endl
       << endl;
  cout << "Boltzmann weight exp(- beta * H ), beta = 1/T " << endl
       << endl;
  cout << "The program uses k_B=1 and mu_B=1 units " << endl;

  // Read seed for random numbers
  int p1, p2;
  ifstream Primes("Primes");
  Primes >> p1 >> p2;
  Primes.close();

  // Read input informations
  ReadInput.open(file_in);
  // ReadInput.open("input_TH.dat");

  ReadInput >> temp; // da ciclo

  if (restart)
    Seed.open("seed.out");
  else
    Seed.open("seed.in");
  Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
  rnd.SetRandom(seed, p1, p2);
  Seed.close();

  beta = 1.0 / temp;
  cout << "Temperature = " << temp << endl;

  ReadInput >> nspin;
  cout << "Number of spins = " << nspin << endl;

  ReadInput >> J;
  cout << "Exchange interaction = " << J << endl;

  ReadInput >> h;
  cout << "External field = " << h << endl
       << endl;

  ReadInput >> metro; // if=1 Metropolis else Gibbs

  out_dir = (metro) ? "output_metro/h_0.0" + to_string(int(h * 100)) : "output_gibbs/h_0.0" + to_string(int(h * 100));

  ReadInput >> nblk;

  ReadInput >> nstep;

  ReadInput >> th_step;

  if (metro)
    cout << "The program perform Metropolis moves" << endl;
  else
    cout << "The program perform Gibbs moves" << endl;
  cout << "Number of blocks = " << nblk << endl;
  cout << "Number of steps in one block = " << nstep << endl
       << endl;
  ReadInput.close();

  // Prepare arrays for measurements
  iu = 0; // Energy
  ic = 1; // Heat capacity
  im = 2; // Magnetization
  ix = 3; // Magnetic susceptibility

  n_props = 4; // Number of observables

  // initial configuration
  if (restart)
  {
    Readconf.open(out_dir + "/config.final");
    for (int i = 0; i < nspin; ++i)
    {
      Readconf >> s[i];
    }
  }
  else
  {
    for (int i = 0; i < nspin; ++i)
    {
      if (rnd.Rannyu() >= 0.5)
        s[i] = 1;
      else
        s[i] = -1;
    }
  }

  // Evaluate energy etc. of the initial configuration
  Measure();

  // Print initial values for the potential energy and virial
  cout << "Initial energy = " << walker[iu] / (double)nspin << endl;
}

void Move(bool metro)
{
  int o;
  double p, energy_old, energy_new, sm;
  double energy_up, energy_down;

  for (int i = 0; i < nspin; ++i)
  {
    attempted++;

    // Select randomly a particle (for C++ syntax, 0 <= o <= nspin-1)
    o = (int)(rnd.Rannyu() * nspin);

    // Metropolis
    if (metro)
    {
      sm = s[o];
      energy_old = Boltzmann(sm, o);
      sm *= -1;
      energy_new = Boltzmann(sm, o);
      p = min(1., exp(-beta * (energy_new - energy_old)));
      if (rnd.Rannyu() < p)
      {
        s[o] = sm;
        accepted++;
      }
    }
    // Gibbs sampling
    else
    {
      energy_up = Boltzmann(1, o);
      energy_down = Boltzmann(-1, o);
      p = 1. / (1. + exp(beta * (energy_up - energy_down)));
      if (rnd.Rannyu() < p)
        s[o] = +1;
      else
        s[o] = -1;
      accepted++;
    }
  }
}

double Boltzmann(int sm, int ip)
{
  double ene = -J * sm * (s[Pbc(ip - 1)] + s[Pbc(ip + 1)]) - h * sm;
  return ene;
}

void Measure()
{
  double u = 0.0, m = 0.0;

  // cycle over spins
  for (int i = 0; i < nspin; ++i)
  {
    u += -J * s[i] * s[Pbc(i + 1)] - 0.5 * h * (s[i] + s[Pbc(i + 1)]);
    m += s[i];
  }
  walker[iu] = u;
  walker[im] = m;
  walker[ix] = m * m;
  walker[ic] = u * u;
}

// Reset block averages
void Reset(int iblk)
{

  if (iblk == 1)
  {
    for (int i = 0; i < n_props; ++i)
    {
      glob_av[i] = 0;
      glob_av2[i] = 0;
    }
  }

  for (int i = 0; i < n_props; ++i)
  {
    blk_av[i] = 0;
  }

  blk_norm = 0;
  attempted = 0;
  accepted = 0;
}

// Update block averages
void Accumulate(void)
{

  for (int i = 0; i < n_props; ++i)
  {
    blk_av[i] = blk_av[i] + walker[i];
  }

  blk_norm = blk_norm + 1.0;
}

// Print results for current block
void Averages(int iblk)
{

  ofstream Ene, Heat, Mag, Chi;

  cout << "Block number " << iblk << endl;
  cout << "Acceptance rate " << accepted / attempted << endl
       << endl;

  // Energy
  stima_u = blk_av[iu] / blk_norm / (double)nspin;
  glob_av[iu] += stima_u;
  glob_av2[iu] += stima_u * stima_u;
  err_u = Error(glob_av[iu], glob_av2[iu], iblk);

  if (iblk == nblk - 1)
  {
    Ene.open(out_dir + "/output_ene.dat", ios::app);
    Ene << temp << "\t" << stima_u << "\t" << glob_av[iu] / (double)iblk << "\t" << err_u << endl;
    Ene.close();
  }

  // Heat Capacity
  stima_c = (blk_av[ic] / blk_norm - pow(blk_av[iu] / blk_norm, 2)) * beta * beta / (double)nspin;
  glob_av[ic] += stima_c;
  glob_av2[ic] += stima_c * stima_c;
  err_c = Error(glob_av[ic], glob_av2[ic], iblk);

  if (iblk == nblk - 1)
  {
    Heat.open(out_dir + "/output_heat.dat", ios::app);
    Heat << temp << "\t" << stima_c << "\t" << glob_av[ic] / (double)iblk << "\t" << err_c << endl;
    Heat.close();
  }

  // Susceptibility
  stima_x = beta * blk_av[ix] / blk_norm / (double)nspin;
  glob_av[ix] += stima_x;
  glob_av2[ix] += stima_x * stima_x;
  err_x = Error(glob_av[ix], glob_av2[ix], iblk);

  if (iblk == nblk - 1)
  {
    Chi.open(out_dir + "/output_x.dat", ios::app);
    Chi << temp << "\t" << stima_x << "\t" << glob_av[ix] / (double)iblk << "\t" << err_x << endl;
    Chi.close();
  }

  // Magnetization
  stima_m = blk_av[im] / blk_norm / (double)nspin;
  glob_av[im] += stima_m;
  glob_av2[im] += stima_m * stima_m;
  err_m = Error(glob_av[im], glob_av2[im], iblk);

  if (iblk == nblk - 1)
  {
    Mag.open(out_dir + "/output_mag.dat", ios::app);
    Mag << temp << "\t" << stima_m << "\t" << glob_av[im] / (double)iblk << "\t" << err_m << endl;
    Mag.close();
  }

  cout << "----------------------------" << endl
       << endl;
}

void ConfFinal(void)
{
  ofstream WriteConf;

  cout << "Print final configuration to file config.final " << endl
       << endl;
  WriteConf.open(out_dir + "/config.final");
  for (int i = 0; i < nspin; ++i)
  {
    WriteConf << s[i] << endl;
  }
  WriteConf.close();

  rnd.SaveSeed();
}

int Pbc(int i) // Algorithm for periodic boundary conditions
{
  if (i >= nspin)
    i = i - nspin;
  else if (i < 0)
    i = i + nspin;
  return i;
}

double Error(double sum, double sum2, int iblk)
{
  if (iblk == 1)
    return 0.0;
  else
    return sqrt((sum2 / (double)iblk - pow(sum / (double)iblk, 2)) / (double)(iblk - 1));
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
