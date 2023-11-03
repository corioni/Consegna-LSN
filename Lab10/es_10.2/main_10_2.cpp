#include "main_10_2.h"
#include "GA.h"
#include <mpi.h> // Include MPI library
#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <sstream>
#include <armadillo>
#include <cstdlib>
#include <cassert>
// #include <filesystem>

using namespace std;
using namespace arma;

int main(int argc, char *argv[])
{
    // Initialize MPI
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // size = number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank = process rank

    // Set the seed for random number generation
    int seed;

    // Generate a seed based on the process rank
    if (rank == 0)
        seed = size + 1;
    else
        seed = rank;

    arma_rng::set_seed(seed);

    /* INITIALIZING FUNDAMENTAL DATA ***************************************************************/

    int N_pop, N_city, N_epoch, N_migr, N_send;
    ifstream ReadInput("input.in");

    if (!ReadInput)
    {
        cerr << "Failed to open input file." << endl;
        return -1;
    }

    // Read the values from the input file
    ReadInput >> N_pop >> N_epoch >> N_migr >> N_send;
    ReadInput.close();
    N_city = file_rows("American_capitals.dat");
    string outpur_dir = "output/" + to_string(N_epoch/N_migr-1) + "_migr/";
    if(rank==0)
        Check_Out_Directory(outpur_dir);


    /* INITIALIZING AND BROADCASTING CITY POSITIONS **************************************************/

    // Initialize matrix for city positions
    mat pos(N_city, 2);

    // Generate city positions if rank is 0
    if (rank == 0)
    {
        assert(load_pos("American_capitals.dat").n_cols == pos.n_cols);
        pos = load_pos("American_capitals.dat");
    }

    // Convert positions from arma::mat to double[] arrays
    double pos_x[N_city];
    double pos_y[N_city];
    for (int i = 0; i < N_city; i++)
    {
        pos_x[i] = pos(i, 0);
        pos_y[i] = pos(i, 1);
    }

    // Broadcast the same positions to all nodes
    if (size > 1)
    {
        MPI_Bcast(pos_x, N_city, MPI_DOUBLE_PRECISION, 0, MPI_COMM_WORLD);
        MPI_Bcast(pos_y, N_city, MPI_DOUBLE_PRECISION, 0, MPI_COMM_WORLD);
    }

    // Initialize each node's positions with the broadcasted values
    for (int i = 0; i < N_city; i++)
    {
        pos(i, 0) = pos_x[i];
        pos(i, 1) = pos_y[i];
    }

    /* INITIALIZING AND BROADCASTING CITY POSITIONS **************************************************/

    // Create the population object
    Population pop(N_pop, N_city, pos);

    // Create arrays and variables for sending and receiving chromosomes
    int best_chromos[N_send * N_city];
    int migrated_chromos[N_send * N_city * size];
    mat config(N_pop, N_city);
    rowvec chromo(N_city);

    /* EVOLUTION LOOP *******************************************************************/
    if (rank == 0)
    {
        cout << "Running the GA program in " << size << " contintent, each computing evolution of " << N_pop << " chromosomes in " << N_epoch << " epochs..." << endl;
    }
    // Open a file for recording results
    ofstream out(outpur_dir + "cost_" + to_string(rank) + "_rank.txt");
    out << "best\t mean" << endl;
    out << pop.Get_Fit()(0) << "\t" << mean(pop.Get_Fit().rows(0, N_pop / 2)) << endl;
    pop.Print(0, outpur_dir + "fist_path_" + to_string(rank) + ".dat");

    for (int i = 0; i < N_epoch; i++)
    {
        pop.Genetic_Step();
        out << pop.Get_Fit()(0) << "\t" << mean(pop.Get_Fit().rows(0, N_pop / 2)) << endl;

        if (rank == 0)
        {
            printProgressBar(static_cast<double>(i) / N_epoch);
        }
        if (i % N_migr == 0 && size > 1)
        {
            // Sort the population by fitness
            pop.Sort();
            config = pop.Get_chromos();

            for (int j = 0; j < N_send; j++)
                for (int h = 0; h < N_city; h++)
                    best_chromos[h + j * N_city] = config(j, h);

            MPI_Allgather(best_chromos,     // send_data
                          N_send * N_city,  // send_count
                          MPI_INTEGER,      // send_datatype
                          migrated_chromos, // recv_data
                          N_send * N_city,  // recv_count
                          MPI_INTEGER,      // recv_datatype
                          MPI_COMM_WORLD);  // communicator

            // Shuffle the order of received individuals in migrated_chromos
            rowvec shuffle_indices = shuffle(regspace<rowvec>(1, N_send * size));

            for (int i = 0; i < N_send * size; i++)
            {
                int shuffled_index = shuffle_indices(i);

                for (int k = 0; k < N_city; k++)
                    chromo(k) = migrated_chromos[k + shuffled_index * N_city];

                config.row(N_pop - i - 1) = chromo;
            }
            pop.Set_chromos(config);
        }
    }
    if (rank == 0)
    {
        cout << endl;
    }
    out.close();
    assert(N_pop == pop.Get_Dim());
    // Save the best path
    pop.Print(0, outpur_dir + "best_path_" + to_string(rank) + ".dat");

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
