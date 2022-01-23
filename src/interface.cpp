/*
 * GAIM interface function cpp file 
 * Copyright (C) 2022  Georgios Detorakis (gdetor@protonmail.com)
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
 * @file interface.cpp
 * Defines an interface function for calling game in a single-line code from 
 * within any other software.
 */
// $Log$
#include "gaim.h"

extern "C"
/**
 * @brief Interface function for calling GAIM from within any other C/C++ 
 * source code of software (libgaim).
 *
 * It initializes all the necessary parameters and vectors for evolving a 
 * population of individuals based on a predetermined Genetic Algorithm (GA). 
 *
 * @param[in] func A pointer function to the objective/fitness function
 * @param[in] n_generations Number of generations   
 * @param[in] population_size Number of individuals within a population
 * @param[in] genome_size How many parameters the fitness function has
 * @param[in] n_offsprings Number of new candidate individuals (children)
 * @param[in] n_replacements Number of parents to be replaced per generation
 * @param[in] n_rounds Number of independent experiments to run
 * @param[in] n_islands Number of islands in a Island Model
 * @param[in] n_immigrants Number of individuals to move from one island to
 *              another (only for IM)
 * @param[in] migration_interval Determines how frequent individuals have to
 *              move from one island to another
 * @param[in] a Vector with genes lower bounds
 * @param[in] b Vector with genes upper bounds
 * @param[in] clipping Gene's clipping method (universal: all the genes are
 * clamped with the same values a and b. Individual: Each gene has its own 
 * clamped values a and b. PPPPPPP
 * @param[in] clipping_fname Filename of clipping file
 * @param[in] experiment_id The name of the current experiment
 * @param[in] log_fname The directory in which all the results and logs will be
 *              saved
 * @param[in] pickup_method Selection method of migrating individuals in IM.
 * Poor: Select individuals with lowest fitness. Elite: select individuals
 * with the highest fitness. Random: select individuals randomly.
 * @param[in] replace_method Replacement method of individuals on islands who
 * receive immigrants. Three method (poor, elite, and random).
 * @param[in] im_graph_fname The filename of the file that contains the
 * connectivity graph for an IM. 
 * @param[in] log_fitness Enable/disable the track of individuals' fitness
 * @param[in] log_average_fitness Enable/disable the track of population
 * (average) fitness
 * @param[in] log_bsf Enable/disable the track of Best So far Fitness (BSF)
 * fitness. 
 * @param[in] log_best_genome Enable/disable the track of the best genome
 * @param[in] is_im_enabled Enable/disable the IM functionality
 * * @return res A py_results_s data structure that contains the average
 * fitness, the BSF, and the best genome. 
 * */
 py_results_s ga_optimization(std::function<REAL_(std::vector<REAL_>&)> func,
                              size_t n_generations,
                              size_t population_size,
                              size_t genome_size,
                              size_t n_offsprings,
                              size_t n_replacements,
                              size_t n_rounds,
                              size_t n_islands,
                              size_t n_immigrants,
                              size_t migration_interval,
                              std::vector<REAL_> a,
                              std::vector<REAL_> b,
                              std::string clipping,
                              std::string clipping_fname,
                              std::string experiment_id,
                              std::string log_fname,
                              std::string pickup_method,
                              std::string replace_method,
                              std::string im_graph_fname,
                              bool log_fitness,
                              bool log_average_fitness,
                              bool log_bsf,
                              bool log_best_genome,
                              bool is_im_enabled) {
    py_results res;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    ga_pms.sel_pms.bias = 1.5;
    ga_pms.sel_pms.selection_method = "random";
    ga_pms.sel_pms.num_parents = 2;
    ga_pms.sel_pms.lower_bound = 1;
    ga_pms.sel_pms.k = 2;
    ga_pms.sel_pms.replace = false;
    
    ga_pms.cross_pms.crossover_method = "one_point";

    ga_pms.mut_pms.mutation_method = "delta";
    ga_pms.mut_pms.mutation_rate = 0.5;
    ga_pms.mut_pms.variance = 0.5;
    ga_pms.mut_pms.low_bound = 1.0;
    ga_pms.mut_pms.up_bound = 1.0;
    ga_pms.mut_pms.time = 1;
    ga_pms.mut_pms.order = 1;
    ga_pms.mut_pms.is_real = true;

    // Assign values to GA parameters
    ga_pms.runs = n_rounds;
    ga_pms.generations = n_generations;
    ga_pms.population_size = population_size;
    ga_pms.num_offsprings = n_offsprings;
    if (n_offsprings >= population_size) {
        printf("Number of offsprings cannot exceed population size!\n");
        exit(-1);
    }
    ga_pms.genome_size = genome_size;
    if (genome_size < 1) { 
        printf("Genome size cannot be smaller than 1!\n");
        exit(-1);
    }
    ga_pms.num_replacement = n_replacements;
    ga_pms.clipping = clipping;
    ga_pms.clipping_fname = clipping_fname;
    ga_pms.a = a;
    ga_pms.b = b;
    
    // Assign values to logging parameters
    pr_pms.print_fitness = log_fitness;
    pr_pms.print_average_fitness = log_average_fitness;
    pr_pms.print_bsf = log_bsf;
    pr_pms.print_best_genome = log_best_genome;
    pr_pms.where2write = log_fname;
    pr_pms.experiment_name = experiment_id;

    if (is_im_enabled) {
        im_pms.num_immigrants = n_immigrants;
        im_pms.num_islands = n_islands;
        im_pms.migration_interval = migration_interval;
        im_pms.pick_method = pickup_method;
        im_pms.replace_method = replace_method;
        im_pms.is_im_enabled = is_im_enabled;
        im_pms.adj_list_fname = im_graph_fname;
    }

    print_parameters(ga_pms, pr_pms, im_pms);
    if (im_pms.is_im_enabled) {
        IM island_model(&im_pms, &ga_pms);
        printf("Optimizing using Island Model!\n");
        for (size_t i = 0; i < im_pms.num_islands; ++i) {
           island_model.island[i].fitness = func;
        }
        island_model.run_islands(&im_pms, &pr_pms);
    } else {
        if (ga_pms.runs == 1) {
            printf("Optimizing using a single GA!\n");
            GA gen_alg(&ga_pms);
            gen_alg.fitness = func;
            gen_alg.evolve(ga_pms.generations, 0, &pr_pms);
            res.bsf = gen_alg.get_bsf();
            res.average_fitness = gen_alg.get_average_fitness();
            res.genome = gen_alg.get_best_genome();
        } else if (ga_pms.runs > 1) {
            printf("Running %d independent GAs!\n", ga_pms.runs);
            independent_runs(&ga_pms, &pr_pms);
        } else {
            printf("Negative number of runs is illegal!\n");
            exit(-1);
        }
    }
    return res;
}
