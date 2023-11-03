
#include "GA.h"
#include "random.h"
#include <fstream>
#include <cmath>
#include <cassert>
#include <armadillo>

using namespace std;
using namespace arma;

Population::Population()
{
    N_pop = 100; // Setting the value of N_pop to 100
    N_city = 34; // Setting the value of N_city to 32

    chromos.set_size(N_pop, N_city); // Creating a matrix of size (100 x 32)

    // Generate a random row vector A containing integers between 2 and N_city
    rowvec A = regspace<rowvec>(2, N_city);

    for (int i = 0; i < N_pop; i++)
    {
        // Initialize each row of chromos with a 1 followed by the values in A
        chromos.row(i)(0) = 1;
        chromos.row(i).subvec(1, N_city - 1) = shuffle(A);
    }

    // Initialize the pos matrix with zeros, size (N_city x 2)
    pos = zeros<mat>(N_city, 2);
    Fitness();
}

Population::Population(int M_pop, int M_city, mat new_pos)
{
    N_pop = M_pop;
    N_city = M_city;

    chromos.set_size(N_pop, N_city);
    fit.set_size(N_pop);
    pos.set_size(N_city, 2);

    rowvec A = regspace<rowvec>(2, N_city);

    for (int i = 0; i < N_pop; i++)
    {
        // Initialize each row of chromos with a 1 followed by the values in A
        chromos.row(i)(0) = 1;
        chromos.row(i).subvec(1, N_city - 1) = shuffle(A);
    }

    pos = new_pos;
    Fitness();
}

Population::~Population() {}

void Population::Check()
{
    // Check if all individuals start from the same city
    colvec ones_column = ones<colvec>(N_pop);
    bool same_start = all(chromos.col(0) == ones_column);

    if (!same_start)
    {
        cout << "Error: Different starting cities detected!" << endl;
        return;
    }

    // Check if each individual visits each city exactly once
    for (int i = 0; i < N_pop; i++)
    {
        rowvec unique_row = unique(chromos.row(i));
        bool all_cities_visited = all(unique_row == regspace<rowvec>(1, N_city));

        if (!all_cities_visited)
        {
            cout << "Error: Some individuals visit the same city multiple times!" << endl;
            return;
        }
        
        // Check if all 34 cities are visited
        if (unique_row.n_elem != N_city)
        {
            cout << "Error: Not all cities are visited!" << endl;
            return;
        }
    }

    cout << "Validation passed: All individuals start from the same city, and all 34 cities are visited exactly once." << endl;
}

void Population::Check(rowvec chromo)
{
    // Check if the individual starts from the same city
    bool same_start = chromo(0) == 1;

    if (!same_start)
    {
        cout << "Error: Different starting city detected!" << endl;
        return;
    }

    // Check if the individual visits each city exactly once
    rowvec unique_row = unique(chromo);
    bool all_cities_visited = all(unique_row == regspace<rowvec>(1, N_city));

    if (!all_cities_visited)
    {
        cout << "Error: The individual visits the same city multiple times or misses some cities!" << endl;
        return;
    }

    // Check if all 34 cities are visited
    if (unique_row.n_elem != N_city)
    {
        cout << "Error: Not all cities are visited!" << endl;
        return;
    }

    cout << "Validation passed: The individual starts from the same city, and all 34 cities are visited exactly once." << endl;
}


double Population::Fitness(rowvec cromo){
    double cost = 0. ;
    int index1 , index2 ;

    for (int i = 0 ; i < N_city - 2 ; i++ ){
        index1 = int(cromo(i)) - 1 ;
        index2 = int(cromo(i+1)) - 1 ;
        cost += sqrt( pow( pos.at(index1 , 0)-pos.at(index2 , 0) , 2. )
                + pow( pos.at(index1 , 1)-pos.at(index2 , 1) , 2. ) ) ;

    }
    cost += sqrt( pow( pos.at( index2 , 0)-pos.at(0 , 0) , 2. ) + pow(pos.at( index2 , 1)-pos.at(0 , 1) , 2. ) );
    //assert(isnormal(cost));
    return cost ;
}

void Population::Fitness()
{
    for (int i = 0; i < N_pop; i++)
    {
        
        fit(i) = 0; // Initialize the fitness value for the current individual.

        for (int j = 0; j < N_city - 1; j++)
        {
            // Calculate the Euclidean distance between consecutive cities in the tour.
            double dx = pos(chromos(i, j) - 1, 0) - pos(chromos(i, j + 1) - 1, 0);
            double dy = pos(chromos(i, j) - 1, 1) - pos(chromos(i, j + 1) - 1, 1);

            // Add the squared distance to the fitness value.
            fit(i) += sqrt(dx * dx + dy * dy);
        }

        // Calculate the squared distance between the last city and the first city in the tour.
        double dx = pos(chromos(i, N_city - 1) - 1, 0) - pos(chromos(i, 0) - 1, 0);
        double dy = pos(chromos(i, N_city - 1) - 1, 1) - pos(chromos(i, 0) - 1, 1);

        // Add the squared distance to the fitness value.
        fit(i) += sqrt(dx * dx + dy * dy);
        
    }
}

void Population::Sort()
{
    Fitness();
    uvec sorted_indices = sort_index(fit, "ascend");

    // Rearrange 'chromos' and 'fit' based on the sorted
    chromos = chromos.rows(sorted_indices);
    fit = fit(sorted_indices);
}

int Population::Select()
{
    Sort();
    double r =0;
    do{
    r = randu<double>();
    }while (r == 1.);

    double r1 = int(N_pop * pow(r, 4.)) + 1.;

    if (r1 == N_pop)
        r1--;

    return r1;
}

rowvec Population::Mutations(rowvec old_chromo)
{
    // Extract the chromosome excluding the first element
    rowvec chromo = old_chromo.cols(1, N_city - 1);

    if (randu<double>() < (0.1))
    {
        PairPermutation(chromo);
        //cout<<" Performed pair mutation"<<endl;
    }

    if (randu<double>() < (0.1))
    {
        ShiftMutation(chromo);
        //cout<<" Performed shift mutation"<<endl;
    }

    if (randu<double>() < (0.1))
    {
        PermutationMutation(chromo);
        //cout<<" Performed permutation mutation"<<endl;
    }

    if (randu<double>() < (0.1))
    {
        InversionMutation(chromo);
        //cout<<" Performed inversion mutation"<<endl;
    }

    // Combine the mutated chromosome with the first element
    rowvec new_chromo = join_horiz(rowvec(1, fill::ones), chromo);
    // Add an assertion to check the column count
    assert(new_chromo.n_cols == old_chromo.n_cols);
    //Check(new_chromo);
    return new_chromo;
}


void Population::PairPermutation(rowvec &chromo)
{
    int a, b;
    do
    {
        a = randi<double>(distr_param(0,chromo.size()-1));
        b = randi<double>(distr_param(0,chromo.size()-1));
    } while (a == b);
    chromo.swap_cols(a, b);
}

void Population::ShiftMutation(rowvec &chromo)
{
    int index = randi<double>(distr_param(1, chromo.size()-2));
    int N = randi<double>(distr_param(1, chromo.size()-1 - index )); // di quante posizioni shiftare
    chromo.subvec(index, chromo.size()-1) = shift(chromo.subvec(index, chromo.size()-1), N);
}

void Population::PermutationMutation(rowvec &chromo)
{
    int m = randi<int>(distr_param(0, (chromo.size()-1) / 2));
    int n1 = randi<int>(distr_param(0, chromo.size()-2));
    int n2 = PBC(n1 + randi<int>(distr_param(m, chromo.size()-1)), chromo.size()-1);

    for (int i = 0; i < m; ++i)
    {
        chromo.swap_cols(PBC(n1 + i, chromo.size()-1), PBC(n2 + i,chromo.size()));
    }
}

void Population::InversionMutation(rowvec &chromo)
{
    int n1 = randi<int>(distr_param(0, chromo.size()-2));
    int n2 = randi<int>(distr_param(n1 + 1, chromo.size()-1));

    for (int i = 0; i < (n2 - n1) / 2; ++i)
    {
        chromo.swap_cols(PBC(n1 + i, chromo.size()), PBC(n2 - i, chromo.size()-1));
    }
}

int Population::PBC(int index, int size)
{
    return (index + size) % size;
}

mat Population::Crossover()
{
    int i_m, i_p;
    do
    {
        i_m = Select();
        i_p = Select();
    } while (i_m == i_p);

    rowvec mum = chromos.row(i_m);
    rowvec dad = chromos.row(i_p);

    mat son(2, N_city, fill::zeros);

    if (randu<double>() < 0.6)
    {
        int i1 = 0;
        int i2 = randi<int>(distr_param(1, N_city - 2));

        rowvec cut_mum = mum.subvec(i1, i2);
        rowvec cut_dad = dad.subvec(i1, i2);

        son.row(0).cols(i1, i2) = cut_dad;
        son.row(1).cols(i1, i2) = cut_mum;

        int j = 0;
        int k = 0;

        for (int i = i2 + 1; i < N_city; i++)
        {
            while (any(cut_dad == mum(j)))
            {
                j++;
            }
            son(0, i) = mum(j);
            j++;

            // Find an available gene from 'dad' for 'son' at index i
            while (any(cut_mum == dad(k)))
            {
                k++;
            }
            son(1, i) = dad(k);
            k++;
        }

        return son;
    }
    else
    {
        return join_cols(mum, dad);
    }

}


 void Population::Genetic_Step()
{
    Sort();
    mat son(2, N_city);
    // the worst half is mutated
    for (int i = N_pop / 2; i < N_pop-1; i += 2)
    {
        son = Crossover();
        //son.brief_print();
         chromos.row(i)= Mutations(son.row(0));
         chromos.row(i+1)=Mutations(son.row(1));
    }
    Sort();
} 


void Population::Evolution(int N_epochs,std::string positions)
{
    cout<<"Computing evolution of "+positions+" in "+to_string(N_epochs)+" epochs ..."<<endl;
    ofstream out1("output/"+positions+"_best_cost.txt");
    ofstream out2("output/"+positions+"_ave_cost.txt");
    out1<<fit(0)<<endl;
    out2<<mean(fit.rows(0,N_pop/2))<<endl;
    for (int i = 0; i < N_epochs; i++)
    {
        Genetic_Step();
        out1<<fit(0)<<endl;
        out2<<mean(fit.rows(0,N_pop/2))<<endl;
        printProgressBar((double)i/N_epochs);
    }
    Check();
    //fit.brief_print();
}

void Population::printProgressBar(double progress)
{
    const int barWidth = 70;
    int pos = barWidth * progress;
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

void Population::Print(int i_row, std::string filename) {
    ucolvec v = arma::conv_to<arma::ucolvec>::from(vectorise(chromos.row(i_row)).t());
    v.save(filename, raw_ascii);

}
