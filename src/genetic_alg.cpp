/*
 * Genetic Algorithm Class cpp file for GAIM software 
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
 * @file genetic_alg.cpp
 * Defines the Genetic Algorithm class which controls evolutionary optimization.
 */
// $Log$
#include "gaim.h"
#include <numeric>


/**
 * @brief Constructor of GA class. 
 *
 * It initializes all the necessary parameters and vectors for evolving a 
 * population of individuals based on a predetermined Genetic Algorithm (GA). 
 *
 * @param[in] ga_pms    A structure that contains all the parameters for the GA
 * @return Nothing
 */
GA::GA(ga_parameter_s *ga_pms)
{
    REAL_ bound_val;
    // alpha and beta are vectors
    alpha = ga_pms->a;  // Lower bound for genes [a, b]
    beta = ga_pms->b;   // Upper bound for genes [a, b]

    // Validate alpha and beta paremeters
    if ((alpha.size() != ga_pms->genome_size) && \
         (beta.size() != ga_pms->genome_size)) {
        std::cout << "Genome limits [a, b] size is not correct!" << std::endl;
        std::cout << "Size of a and b = genome size!" << std::endl;
        exit(-1);
    }

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> genes(alpha[0], beta[0]);

    // Assign and validate mu values
    mu = ga_pms->population_size;   // Population size
    if (mu < 2) {
        std::cout << "Population size is smaller than 2!" << std::endl; 
        std::cout << "Impossible to have sexual reproduction!" << std::endl;
        exit(-1);
    }
    // Assign and validate lambda values
    lambda = ga_pms->num_offsprings;    // Number of offspring
    replace_perc = ga_pms->num_replacement; // Number of individuals being replaced
    if (replace_perc > lambda) {
        std::cout << "Generation replacement dimension is larger than \
            offsprings population size!" << std::endl;
        exit(-1);
    }
    genome_size = ga_pms->genome_size;  // Genome size

    fitness = sphere;  // Define the cost function (example -> sphere)

    // Initialize selection method
    selection_method = ga_pms->sel_pms.selection_method;
    bias = ga_pms->sel_pms.bias;
    num_parents = ga_pms->sel_pms.num_parents;
    lower_bound = ga_pms->sel_pms.lower_bound;
    k = ga_pms->sel_pms.k;
    replace = ga_pms->sel_pms.replace;

    select_selection_method();

    // Initialize crossover method
    crossover_method = ga_pms->cross_pms.crossover_method;
    select_crossover_method();

    // Initialize mutation method
    mutation_method = ga_pms->mut_pms.mutation_method;
    mutation_rate = ga_pms->mut_pms.mutation_rate;
    variance = ga_pms->mut_pms.variance;
    low_bound = ga_pms->mut_pms.low_bound;
    up_bound = ga_pms->mut_pms.up_bound;
    time = ga_pms->mut_pms.time;
    order = ga_pms->mut_pms.order;
    is_real = ga_pms->mut_pms.is_real;

    select_mutation_method();

    // Initialize the population vector
    for (size_t i = 0; i < mu; ++i) {
        population.push_back(individual_s());
        population[i].id = i;               // Unique ID
        population[i].fitness = -10000;     // Fitness value
        population[i].is_selected = false;
        for (size_t j = 0; j < genome_size; ++j) {
            population[i].genome.push_back(genes(gen)); // Genome
            genes = std::uniform_real_distribution<>(alpha[j], beta[j]);
        }
    }
    sorted_population = population; // Sort the population based on fitness

    // Initialize the offsprings vector
    for (size_t i = 0; i < lambda; ++i) {
        offsprings.push_back(individual_s());
        offsprings[i].id = i;
        offsprings[i].fitness = -10000;
        for (size_t j = 0; j < genome_size; ++j) {
            offsprings[i].genome.push_back(genes(gen)); 
            genes = std::uniform_real_distribution<>(alpha[j], beta[j]);
        }
    }

    // corresponding file (see config file)
    if (ga_pms->clipping == "file") {
        auto ifile = std::fstream(ga_pms->clipping_fname, std::ios::in);
        if (!ifile) {
            std::cout << "Unable to open file " << ga_pms->clipping_fname\
                << std::endl;
            exit(1);
        }
        
        for (size_t i = 0; i < mu; ++i) {
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                ifile >> bound_val;
                population[i].lower_limit.push_back(bound_val);
            }
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                ifile >> bound_val;
                population[i].upper_limit.push_back(bound_val);
            }
        }
    } else if (ga_pms->clipping == "individual") {
        for (size_t i = 0; i < mu; ++i) {
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                population[i].lower_limit.push_back(alpha[j]);
            }
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                population[i].upper_limit.push_back(beta[j]);
            }
        }
    } else {
        for (size_t i = 0; i < mu; ++i) {
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                population[i].lower_limit.push_back(alpha[0]);
            }
            for (size_t j = 0; j < ga_pms->genome_size; ++j) {
                population[i].upper_limit.push_back(beta[0]);
            }
        }
    }
}


/**
 * @brief Destructor of GA class
 */
GA::~GA()
{
    population.clear();
    offsprings.clear();
    sorted_population.clear();
}


/**
 * Evaluates the fitness of each individual based on a predefined cost
 * function. 
 *
 * @param[in] x Vector of individuals (population)
 * @return Nothing (void)
 *
 * @see test_functions.cpp
 */
void GA::evaluation(std::vector<individual_s> &x)
{
    for (size_t i = 0; i < x.size(); ++i) {
        x[i].fitness = fitness(&x[i].genome[0], x[i].genome.size());
    }
}


/**
 * Sorts the individuals within a population based on their fitness value.
 *
 * @param  void
 * @return Nothing (void)
 */
void GA::sort_population(void)
{
    sorted_population = population;
    std::sort(sorted_population.begin(),
              sorted_population.end(),
              compare_fitness);
}


/**
 * The basic operation for forming the next generation. It determines which 
 * offspring individuals will pass their genomes to the next generation and 
 * which parents will be replaced. By default, it replaces the worst parents
 * by the best offspring. 
 *
 * @param[in] perc The percentage of old individuals that will be replaced by
 * new individuals.
 * @return Nothing (void)
 *
 * @note The user can define his or her own method for the next generation.
 */
void GA::next_generation(size_t perc=3)
{
    std::sort(population.begin(), population.end(), compare_fitness);
    std::sort(offsprings.begin(), offsprings.end(), compare_fitness);
    if (perc > lambda) {
        std::cout << "Percentage of offspring is larger than the\
            available number of offspring!" << std::endl;
        exit(-1);
    }
    for (size_t i = 0, j = lambda-1; i < perc; ++i, --j) {
        population[i].genome = offsprings[j].genome;
        population[i].fitness = offsprings[j].fitness;
    }
}


/**
 * Resets selection flags. A selection flags indicate if an individual has been
 * selected to procreate from any selection method. This method has to be used
 * only when replace is set to true in selection method.
 *
 * @param[in] (void)
 * @return Nothing (void)
 *
 * @warning This methods has to be called after the method next_generation and
 * only when a selection operator without replacement has been chosen.
 *
 */
void GA::reset_selection_flags()
{
    for (auto &ind : population) {
        ind.is_selected = false;
    }
}


/**
 * Clips the value of genes indipendently based on predermined lower and upper
 * boundaries.
 *
 * @param[in] (void)
 * @return Nothing (void)
 *
 * @warning This methods has to be called only when the lower and upper
 * boundaries (limits) for every individual's genome have been initialized
 * properly.
 */
void GA::clip_genome()
{
    // Alternative we could use replace_if
    for (auto &ind : population) {
        for (size_t i = 0; i < ind.genome.size(); ++i) {
            if (ind.genome[i] > ind.upper_limit[i]) { ind.genome[i] = ind.upper_limit[i];}
            if (ind.genome[i] < ind.lower_limit[i]) { ind.genome[i] = ind.lower_limit[i];}
        }
    }
}


#ifndef CUSTOM
/**
 *  @brief One single step of evolution (this is the main GA algorithm).
 *
 *  Runs one step of evolution. This means that this method evaluates and 
 *  sorts a population of individuals. It then selects parents, crosses and
 *  mutates their genomes, and generates the proper amount of offspring.
 *  Then it replaces the old generation with a new one. 
 *  
 *  @param[in] (void)
 *  @return Nothing (void)
 */
void GA::run_one_generation(void)
{
    /* int order; */
    /* size_t xover; */
    individual_s parent1, parent2;
    std::vector<individual_s> parents;
    std::vector<REAL_> child;

    // Iteration index
    static size_t iter_index = 0;

    // Evaluate fitness of each individual
    evaluation(population);

    // Sort the entire population based on fitness
    sort_population();

    // Bookkeeping
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

    // Generate new offspring
    for(size_t i = 0; i < lambda-1; ++i) {
        // Parents selection
        // parents = selection(2, 2, false);
        parents = (this->*selection)(population);
        parent1 = parents[0];
        parent2 = parents[1];
        /* parent1 = parents[i]; */
        /* parent2 = parents[i+1]; */
       
        // Reset the selection flags 
        // This has to be placed outside the loop in case the selection method
        // is placed outside the loop
        reset_selection_flags();

        // Crossover
        child = (this->*crossover)(parent1.genome, parent2.genome);
        // child = order_one_crossover(parent1.genome, parent2.genome);

        // Mutation
        child = (this->*mutation)(child);

        // Append the offspring genome list
        offsprings[i].genome = child;
    }
    // Evaluate offspring fitness
    evaluation(offsprings);

    // Integrate offspring in the initial population
    next_generation(replace_perc);
    
    // Clip genome
    clip_genome();
    
    // Increase iteration index
    iter_index++;
}
#endif

/**
 *  @brief Evolves a population based on previously defined operators.
 *
 *  Evolves a population of individuals based on operations such as selection,
 *  crossover, and mutation that have been predefined. It executes the  
 *  evolutionary step over the total number of generations. 
 *  Finally, it prints out results either to STDOUT or to file.
 *
 * @param[in] generations   Total number of generations
 * @param[in] unique_id     Unique ID of a thread in case Island Model or
 *                          Independent runs are used. Otherwise, can be any
 *                          number of type size_t
 * @param[in] pms           Structure of logging parameters
 * @return Nothing (void)   
 */
void GA::evolve(size_t generations,
                size_t unique_id,
                pr_parameter_s *pms)
{
#ifdef TIME
    auto start = std::chrono::high_resolution_clock::now();
#endif
    for(size_t i = 0; i < generations; ++i) {
        run_one_generation();
    }
    sort_population();
#ifdef TIME
    auto end = std::chrono::high_resolution_clock::now();
    auto taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Total time: " << taken.count() << " ms "<< std::endl;
#endif

    if (pms->print_fitness) {
        print_fitness(population, unique_id, pms->where2write);
    }
    if (pms->print_average_fitness) {
        print_avg_fitness(fit_avg, unique_id, pms->where2write);
    }
    if (pms->print_bsf) {
        print_bsf(bsf, unique_id, pms->where2write);
    } 
    if (pms->print_best_genome) {
        print_best_genome(best_individual.genome, unique_id, pms->where2write);
    }
}
