/* GAIM Custom GA example file for GAIM software 
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
 * @file main_lib.cpp
 * How to modify the run_one_generation method to run a custom GA. 
 */
// $Log$
#include "gaim.h"
#include <iostream>



/*
 * Example of modifying GA algorithm.
 * This is the main algororithm that the end-user can (should) modify based on
 * her/his optimization problem. The available operators can be found in
 * documentation or in the source files under the files with corresponding 
 * names. Of course the user is free to develop their own operators and use 
 * them here. 
 */
void GA::run_one_generation()
{
    /* int order; */
    /* std::size_t xover; */
    individual_s parent1, parent2;
    std::vector<individual_s> parents;
    std::vector<REAL_> child;

    // Evaluate fitness of each individual
    evaluation(population);

    // Sort the entire population based on fitness
    sort_population();


    best_individual = sorted_population[sorted_population.size()-1];
    hfi.push_back(best_individual.fitness);   // Best fitness
    bsf_genome = best_individual.genome;  // Best so far genome
    lfi.push_back(sorted_population[0].fitness);    // Lowest fitness
    
    // Average fitness across population
    REAL_ acc = std::accumulate(population.begin(),
                                population.end(),
                                0,
                                average_fitness);
    fit_avg.push_back(acc / static_cast<REAL_>(mu));
    
    bsf.push_back(best_individual.fitness);

    // Generate new offsprings
    for(std::size_t i = 0; i < lambda; ++i) {
        // Parents selection
        parents = selection(2, 2, false);
        parent1 = parents[0];
        parent2 = parents[1];
     
        // Crossover
        // child = crossover(parent1.genome, parent2.genome, xover, order);
        child = crossover(parent1.genome, parent2.genome);

        // Mutation
        child = mutation(child, 0.5, 0.5);

        // Append the offsprings genome list
        offsprings[i].genome = child;
    }
    // Evaluate offsprings fitness
    evaluation(offsprings);
   
    // Integrate offsprings in the initial population
    next_generation(replace_perc);
    
    // Clip genome
    clip_genome();
}



/*
 * This file demonstrates how the GAIM library can be called from a C++ 
 * code. 
 */
int main() {
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    // Read the parameters from a configuration file
    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("../examples/demo.cfg");
    // Print all the parameters either on stdout or to a file
    print_parameters(ga_pms, pr_pms, im_pms);

    // Instantiate GA class
    GA gen_alg(&ga_pms);

    // Run an evolution experiment
    gen_alg.evolve(ga_pms.generations, 0, &pr_pms);

    return 0;
}

