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
#include <string>
#include <iomanip>
#include "MD_MC.h"

using namespace std;

int main()
{

  string states[3] = {"gas", "liquid","solid"}; //for final simulation
  

  for (int i = 0; i < 3; i++)
  {
    state = states[i];
    cout<<"-------------------------------------------"<<endl;
    cout<<"Performing MD/MC simulations via a Lennard-Jones model if Argon in a "<<state<<" state"<<endl;

    dir_in = state+"_simulation/";
    Input(); // Inizialization
    dir_out = (iNVET) ? (state+"_simulation/MC/") : (state+"_simulation/MD/");

    // Thermalization
    if(!restart){
    for (int step = 1; step <= th_steps; step++)
    {
      Move();
    }}

    int nconf = 1;
    for (int iblk = 1; iblk <= nblk; iblk++) // Simulation
    {
      Reset(iblk); // Reset block averages
      for (int istep = 1; istep <= nstep; istep++)
      {
        Move();
        Measure();
        Accumulate(); // Update block averages
        if (istep % 10 == 0)
        {
          //      ConfXYZ(nconf);//Write actual configuration in XYZ format //Commented to avoid "filesystem full"!
          nconf += 1;
        }
      }
      Averages(iblk); // Print results for current block
    }
    ConfFinal(); // Write final configuration
  }
  return 0;
}

void Input()
{
  ifstream ReadInput, ReadConf, ReadVelocity, Primes, Seed;

  cout << "Classic Lennard-Jones fluid        " << endl;
  cout << "MD(NVE) / MC(NVT) simulation       " << endl
       << endl;
  cout << "Interatomic potential v(r) = 4 * [(1/r)^12 - (1/r)^6]" << endl
       << endl;
  cout << "Boltzmann weight exp(- beta * sum_{i<j} v(r_ij) ), beta = 1/T " << endl
       << endl;
  cout << "The program uses Lennard-Jones units " << endl;

  // Read seed for random numbers
  int p1, p2;
  Primes.open("Primes");
  Primes >> p1 >> p2;
  Primes.close();

  // Read input informations
  ReadInput.open(dir_in+"input." + state);

  ReadInput >> iNVET;
  ReadInput >> restart;

  if (restart)
    Seed.open(dir_out+"seed.out");
  else
    Seed.open("seed.in");
  Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
  rnd.SetRandom(seed, p1, p2);
  Seed.close();

  ReadInput >> temp;
  beta = 1.0 / temp;
  cout << "Temperature = " << temp << endl;

  ReadInput >> npart;
  cout << "Number of particles = " << npart << endl;

  ReadInput >> rho;
  cout << "Density of particles = " << rho << endl;
  vol = (double)npart / rho;
  box = pow(vol, 1.0 / 3.0);
  cout << "Volume of the simulation box = " << vol << endl;
  cout << "Edge of the simulation box = " << box << endl;

  ReadInput >> rcut;
  cout << "Cutoff of the interatomic potential = " << rcut << endl
       << endl;

  vtail = (8.0*pi*rho)/(9.0*pow(rcut,9)) - (8.0*pi*rho)/(3.0*pow(rcut,3));
  ptail = (32.0*pi*rho)/(9.0*pow(rcut,9)) - (16.0*pi*rho)/(3.0*pow(rcut,3));
  cout << "Tail correction for the potential energy = " << vtail << endl;
  cout << "Tail correction for the pressure           = " << ptail << endl;

  ReadInput >> delta;
  if (!iNVET) delta = 0.0005;

  ReadInput >> nblk;

  ReadInput >> nstep;

  ReadInput >> th_steps;

  cout << "The program perform Metropolis moves with uniform translations" << endl;
  cout << "Moves parameter = " << delta << endl;
  cout << "Number of blocks = " << nblk << endl;
  cout << "Number of steps in one block = " << nstep << endl;
  cout << "Steps to reach thermalization of the system= " << th_steps << endl
       << endl;
  ReadInput.close();

  // Prepare arrays for measurements
  iv = 0;      // Potential energy
  iw = 1;      // pressure
  it = 2;
  n_props = 3; // Number of observables

  ig = 3; //index for g(r). Da qui in poi metto i 100 bin in walker
  nbins = 100;
  n_props = n_props + nbins;
  bin_size = (box/2.0)/(double)nbins;

  // Read initial configuration
  cout << "Read initial configuration" << endl
       << endl;
  if (restart)
  {
    ReadConf.open(dir_out + "config.out");
    ReadVelocity.open(dir_out + "velocity.out");
    for (int i = 0; i < npart; ++i)
      ReadVelocity >> vx[i] >> vy[i] >> vz[i];
  }
  else
  {
    ReadConf.open("config.in");
    cout << "Prepare velocities with center of mass velocity equal to zero " << endl;
    double sumv[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < npart; ++i)
    {
      vx[i] = rnd.Gauss(0., sqrt(temp));
      vy[i] = rnd.Gauss(0., sqrt(temp));
      vz[i] = rnd.Gauss(0., sqrt(temp));
      sumv[0] += vx[i];
      sumv[1] += vy[i];
      sumv[2] += vz[i];
    }
    for (int idim = 0; idim < 3; ++idim)
      sumv[idim] /= (double)npart;
    double sumv2 = 0.0, fs;
    for (int i = 0; i < npart; ++i)
    {
      vx[i] = vx[i] - sumv[0];
      vy[i] = vy[i] - sumv[1];
      vz[i] = vz[i] - sumv[2];
      sumv2 += vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i];
    }
    sumv2 /= (double)npart;
    fs = sqrt(3 * temp / sumv2); // fs = velocity scale factor
    cout << "velocity scale factor: " << fs << endl
         << endl;
    for (int i = 0; i < npart; ++i)
    {
      vx[i] *= fs;
      vy[i] *= fs;
      vz[i] *= fs;
    }
  }

  for (int i = 0; i < npart; ++i)
  {
    ReadConf >> x[i] >> y[i] >> z[i];
    x[i] = Pbc(x[i] * box);
    y[i] = Pbc(y[i] * box);
    z[i] = Pbc(z[i] * box);
  }
  ReadConf.close();

  for (int i = 0; i < npart; ++i)
  {
    if (iNVET)
    {
      xold[i] = x[i];
      yold[i] = y[i];
      zold[i] = z[i];
    }
    else
    {
      xold[i] = Pbc(x[i] - vx[i] * delta);
      yold[i] = Pbc(y[i] - vy[i] * delta);
      zold[i] = Pbc(z[i] - vz[i] * delta);
    }
  }

  // Evaluate properties of the initial configuration
  Measure();

//Print initial values for the potential energy and virial
  cout << "Initial potential energy (with tail corrections) = " << walker[iv]/(double)npart + vtail << endl;
  cout << "Initial Pressure         (with tail corrections) = " << rho * temp + (walker[iw] + (double)npart * ptail) / vol << endl << endl;
  return;
}

void Move()
{
  int o;
  double p, energy_old, energy_new;
  double xnew, ynew, znew;

  if (iNVET) // Monte Carlo (NVT) move
  {
    for (int i = 0; i < npart; ++i)
    {
      // Select randomly a particle (for C++ syntax, 0 <= o <= npart-1)
      o = (int)(rnd.Rannyu() * npart);

      // Old
      energy_old = Boltzmann(x[o], y[o], z[o], o);

      // New
      x[o] = Pbc(x[o] + delta * (rnd.Rannyu() - 0.5));
      y[o] = Pbc(y[o] + delta * (rnd.Rannyu() - 0.5));
      z[o] = Pbc(z[o] + delta * (rnd.Rannyu() - 0.5));

      energy_new = Boltzmann(x[o], y[o], z[o], o);

      // Metropolis test
      p = exp(beta * (energy_old - energy_new));
      if (p >= rnd.Rannyu())
      {
        // Update
        xold[o] = x[o];
        yold[o] = y[o];
        zold[o] = z[o];
        accepted = accepted + 1.0;
      }
      else
      {
        x[o] = xold[o];
        y[o] = yold[o];
        z[o] = zold[o];
      }
      attempted = attempted + 1.0;
    }
  }
  else // Molecular Dynamics (NVE) move
  {
    double fx[m_part], fy[m_part], fz[m_part];

    for (int i = 0; i < npart; ++i)
    { // Force acting on particle i
      fx[i] = Force(i, 0);
      fy[i] = Force(i, 1);
      fz[i] = Force(i, 2);
    }

    for (int i = 0; i < npart; ++i)
    { // Verlet integration scheme

      xnew = Pbc(2.0 * x[i] - xold[i] + fx[i] * pow(delta, 2));
      ynew = Pbc(2.0 * y[i] - yold[i] + fy[i] * pow(delta, 2));
      znew = Pbc(2.0 * z[i] - zold[i] + fz[i] * pow(delta, 2));

      vx[i] = Pbc(xnew - xold[i]) / (2.0 * delta);
      vy[i] = Pbc(ynew - yold[i]) / (2.0 * delta);
      vz[i] = Pbc(znew - zold[i]) / (2.0 * delta);

      xold[i] = x[i];
      yold[i] = y[i];
      zold[i] = z[i];

      x[i] = xnew;
      y[i] = ynew;
      z[i] = znew;

      accepted = accepted + 1.0;
      attempted = attempted + 1.0;
    }
  }
  return;
}

double Boltzmann(double xx, double yy, double zz, int ip)
{
  double ene = 0.0;
  double dx, dy, dz, dr;

  for (int i = 0; i < npart; ++i)
  {
    if (i != ip)
    {
      // distance ip-i in pbc
      dx = Pbc(xx - x[i]);
      dy = Pbc(yy - y[i]);
      dz = Pbc(zz - z[i]);

      dr = dx * dx + dy * dy + dz * dz;
      dr = sqrt(dr);

      if (dr < rcut)
      {
        ene += 1.0 / pow(dr, 12) - 1.0 / pow(dr, 6);
      }
    }
  }

  return 4.0 * ene;
}

double Force(int ip, int idir)
{ // Compute forces as -Grad_ip V(r)
  double f = 0.0;
  double dvec[3], dr;

  for (int i = 0; i < npart; ++i)
  {
    if (i != ip)
    {
      dvec[0] = Pbc(x[ip] - x[i]); // distance ip-i in pbc
      dvec[1] = Pbc(y[ip] - y[i]);
      dvec[2] = Pbc(z[ip] - z[i]);

      dr = dvec[0] * dvec[0] + dvec[1] * dvec[1] + dvec[2] * dvec[2];
      dr = sqrt(dr);

      if (dr < rcut)
      {
        f += dvec[idir] * (48.0 / pow(dr, 14) - 24.0 / pow(dr, 8)); // -Grad_ip V(r)
      }
    }
  }

  return f;
}

void Measure() // Properties measurement
{


  int bin;
  double v = 0.0, kin = 0.0, w = 0.0;
  double vij, wij;
  double dx, dy, dz, dr;

//reset the hystogram of g(r)
  for (int k=ig; k<ig+nbins; ++k) walker[k]=0.0;

  // cycle over pairs of particles
  for (int i = 0; i < npart - 1; ++i)
  {
    for (int j = i + 1; j < npart; ++j)
    {
      // distance i-j in pbc
      dx = Pbc(x[i] - x[j]);
      dy = Pbc(y[i] - y[j]);
      dz = Pbc(z[i] - z[j]);

      dr = dx * dx + dy * dy + dz * dz;
      dr = sqrt(dr);
      
      bin = int( dr / bin_size );
      walker[ bin + ig ] += 2.0 ;

      if (dr < rcut)
      {
        vij = 1.0 / pow(dr, 12) - 1.0 / pow(dr, 6);
        wij = 1.0 / pow(dr, 12) - 0.5 / pow(dr, 6);
        v += vij;
        w += wij;
      }
    }
  }



  walker[iv] = 4.0 * v;                                         // Potential energy
  walker[iw] = 48.0 * w / 3.0;



  return;
}

void Reset(int iblk) // Reset block averages
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

void Accumulate(void) // Update block averages
{

  for (int i = 0; i < n_props; ++i)
  {
    blk_av[i] = blk_av[i] + walker[i];
  }
  blk_norm = blk_norm + 1.0;
}

void Averages(int iblk){
   ofstream G, Epot, Pres ;

  const int wd =16;
    cout << "Block number " << iblk << endl;
    cout << "Acceptance rate " << accepted/attempted << endl << endl;

    Epot.open(dir_out+"output_epot.dat",ios::app) ;
    Pres.open(dir_out+"output_pres.dat",ios::app) ;
    G.open(dir_out+"output_g.dat",ios::app) ;

    stima_pot = blk_av[iv]/blk_norm/(double)npart + vtail; //Potential energy
    glob_av[iv] += stima_pot;
    glob_av2[iv] += stima_pot*stima_pot;
    err_pot=Error(glob_av[iv],glob_av2[iv],iblk);

    stima_pres = rho * temp + (blk_av[iw]/blk_norm + ptail * (double)npart) / vol; //Pressure
    glob_av[iw] += stima_pres;
    glob_av2[iw] += stima_pres*stima_pres;
    err_press=Error(glob_av[iw],glob_av2[iw],iblk);

    double dv ; 


    for (int i = ig ; i < ig+nbins  ; i++ ){
        dv = 4.*M_PI*( pow(bin_size*(double)(i+1.), 3. ) - pow( bin_size*(double)(i), 3.))/3.;
        glob_av[i] +=  blk_av[i]/blk_norm/(rho * dv * (double)npart) ;
        glob_av2[i] += pow( blk_av[i]/blk_norm/(rho * dv * (double)npart) , 2. ) ;
        err_gdir = Error(glob_av[i],glob_av2[i],iblk);
    }

//Potential energy per particle
    Epot << setw(wd) << iblk <<  setw(wd) << stima_pot << setw(wd) << glob_av[iv]/(double)iblk << setw(wd)<< err_pot << endl;
//Pressure
    Pres << setw(wd) << iblk <<  setw(wd) << stima_pres << setw(wd) << glob_av[iw]/(double)iblk << setw(wd) << err_press << endl;

//Final histogram
    if ( iblk == nblk ){
        for (int i = ig ; i < ig+nbins  ; i++ ){
            G << glob_av[i]/(double)iblk << " , " << err_gdir << endl ;
        }
    }

    cout << "----------------------------" << endl << endl;



    Epot.close();
    Pres.close();
    G.close();
}





void ConfFinal(void)
{
  ofstream WriteConf, WriteVelocity, WriteSeed;

  cout << "Print final configuration to file config.out" << endl
       << endl;
  WriteConf.open(dir_out + "config.out");
  WriteVelocity.open(dir_out + "velocity.out");
  for (int i = 0; i < npart; ++i)
  {
    WriteConf << x[i] / box << "   " << y[i] / box << "   " << z[i] / box << endl;
    WriteVelocity << vx[i] << "   " << vy[i] << "   " << vz[i] << endl;
  }
  WriteConf.close();
  WriteVelocity.close();

  rnd.SaveSeed();
}

void ConfXYZ(int nconf)
{ // Write configuration in .xyz format
  ofstream WriteXYZ;

  WriteXYZ.open("frames/config_" + to_string(nconf) + ".xyz");
  WriteXYZ << npart << endl;
  WriteXYZ << "This is only a comment!" << endl;
  for (int i = 0; i < npart; ++i)
  {
    WriteXYZ << "LJ  " << Pbc(x[i]) << "   " << Pbc(y[i]) << "   " << Pbc(z[i]) << endl;
  }
  WriteXYZ.close();
}

double Pbc(double r) // Algorithm for periodic boundary conditions with side L=box
{
  return r - box * rint(r / box);
}

double Error(double sum, double sum2, int iblk)
{
  return sqrt(fabs(sum2 / (double)iblk - pow(sum / (double)iblk, 2)) / (double)iblk);
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
