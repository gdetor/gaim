/* GAIM MPI Independent runs of a GA -- C++ example file 
 * Copyright (C) 2019  Georgios Detorakis (gdetor@protonmail.com)
 *                     Andrew Burton (ajburton@uci.edu)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// $Id$
/**
 * @file main_mpi_independent_runs.cpp
 * How to use MPI to run independent GA runs if it's required (e.g. on a
 * cluster).
 */
// $Log$
#include <gaim.h>
#include <mpi.h>

#define OMPI_SKIP_MPICXX


int main(int argc, char **argv)
{
    int rank, size;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Get the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Get the size of the world
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define all the necessary parameters structures
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    // Read parameters from configuration file
    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("demo_independent_runs.cfg");
    
    // Instantiate GA class
    GA gen_alg(&ga_pms);
   
    // Set the fitness function
    gen_alg.fitness = &sphere;

    // Run the GA
    gen_alg.evolve(ga_pms.generations, rank, &pr_pms);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
