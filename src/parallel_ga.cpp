/* parallel_ga.cpp file of GAIM package. This file contains a function to 
 * run independent GAs.
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
 * @file parallel_ga.cpp
 * Implements parallel independent runs of a GA within an experiment so that you can average over separate evolutionary trajectories. 
 */
// $Log$
#include "gaim.h"
#include <thread>


/**
 * @brief It runs X independent GAs in parallel using threads. 
 *
 * Runs an X number of GAs in parallel and independently using threads.
 * @param[in] func A pointer to the fitness function
 * @param[in] ga_pms Structure of GA parameters
 * @param[in] pr_pms Structure of printing parameters
 * @param[in] return_type A string that determines which run the genome will be
 * returned. The available options are:
 *  (*) "minimum" - the genome with the minimum Euclidean distance is returned
 *  (*) "maximum" - the genome with the maximum Euclidean distance is returned
 *  (*) "random" - a randomly chosen genome is returned
 *
 * @return A data structure of type ga_results_s with the selected genome.
 */
ga_results independent_runs(REAL_ (*func)(REAL_ *, size_t),
                            ga_parameter_s *ga_pms,
                            pr_parameter_s *pr_pms,
                            std::string return_type)
{
    std::vector<GA> ind_population(ga_pms->runs, GA(ga_pms));
    std::vector<std::thread> run;
    ga_results_s best_results;

    /// Create the data directory if it doesn't exist
    if (make_dir(pr_pms->where2write)) {
        std::cout << "ERROR: Cannot create directory " << pr_pms->where2write << "\n";
        exit(-1);
    }

    /// Set the fitness function for every run
    for (int i = 0; i < ga_pms->runs; ++i) {
        ind_population[i].fitness = func;
    }

    /// Instantiate threads and GAs
    for(int i = 0; i < ga_pms->runs; ++i) {
        run.push_back(std::thread(&GA::evolve,
                                  &ind_population[i],
                                  ga_pms->generations,
                                  i,
                                  pr_pms));
    }

    /// Join threads
    for(std::thread& th : run) {
        if (th.joinable()) { th.join(); }
	}

    /// Choose from all the runs which genome to return
    best_results = return_best_results(ind_population, return_type);
    return best_results;
}
