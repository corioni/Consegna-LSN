#ifndef __GA_h__
#define __GA_h__

#include <armadillo>

using namespace arma;

class Population
{
public:
    // constructor and dis
    Population();
    Population(int, int, mat);
    ~Population();

    int Get_Dim() { return N_pop; };
    int Get_N_city() { return N_city; };
    mat Get_chromos() { return chromos; };
    void Set_chromos(mat new_chromos) { chromos=new_chromos; };
    mat Get_Cities() { return pos; };
    mat Get_Fit() { return fit; };

    void Check();
        void Check(rowvec);
    void Fitness();
    double Fitness(rowvec cromo);
    void Sort();

    void PairPermutation(rowvec &chromo);
    void ShiftMutation(rowvec &chromo);
    void PermutationMutation(rowvec &chromo);
    void InversionMutation(rowvec &chromo);

    int PBC(int index, int size);
    int Select();
    rowvec Mutations(rowvec chromo);
    mat Crossover();
    void Genetic_Step();
    void printProgressBar(double progress);
    void Print(int i_row, std::string filename);

private:
    int N_city;
    int N_pop;
    mat chromos;
    mat pos;
    vec fit;
};

#endif