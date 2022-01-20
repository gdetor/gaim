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
 * @param[in] pms Structure of GA parameters.
 * @param[in] pr_pms Structure of printing parameters 
 *
 * @return Nothing (void)
 */
void independent_runs(ga_parameter_s *ga_pms,
                      pr_parameter_s *pr_pms)
{
    std::vector<GA> ind_population(ga_pms->runs, GA(ga_pms));
    std::vector<std::thread> run;

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
}
