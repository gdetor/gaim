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

    if ((alpha.size() != ga_pms->genome_size) && \
         (beta.size() != ga_pms->genome_size)) {
        std::cout << "Genome limits [a, b] size is not correct!" << std::endl;
        std::cout << "Size of a and b = genome size!" << std::endl;
        exit(-1);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> genes(alpha[0], beta[0]);

    mu = ga_pms->population_size;   // Population size
    if (mu < 2) {
        std::cout << "Population size is smaller than 2!" << std::endl; 
        std::cout << "Impossible to have sexual reproduction!" << std::endl;
        exit(-1);
    }
    lambda = ga_pms->num_offsprings;    // Number of offspring
    replace_perc = ga_pms->num_replacement; // Number of individuals being replaced
    if (replace_perc > lambda) {
        std::cout << "Generation replacement dimension is larger than \
            offsprings population size!" << std::endl;
        exit(-1);
    }
    genome_size = ga_pms->genome_size;  // Genome size

    fitness = sphere;  // Define the cost function (example -> sphere)

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


#ifdef PYTHON
#ifdef OLDGCC
/**
 * Fitness callback function. Allows the user to define a fitness function on
 * Python side and use it with pygaim (based on pybind11).
 *
 * @param[in] func Functional that takes a vector as input and returns a REAL_.
 * Essentialy this is the fitness function. 
 *              
 * @return True (bool)
 *
 * @see pygaim.cpp, pygaim_demo.py
 */
bool GA::pycallback(std::function<REAL_(std::vector<REAL_>&)> func)
#else
bool GA::pycallback(Callback func)
#endif
{
    fitness = func;
    return true;
}
#endif


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
        x[i].fitness = fitness(x[i].genome);
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
 * Applies a selection operator by selecting two parents for breeding out
 * of a whole population of individuals. By default this method implements a
 * k-tournament selection. 
 *
 * @param[in] num_parents Number of individuals being selected for
 * reproduction.
 * @param[in] k An integer that defines the tournament size (how many 
 *              individuals from the population will be drawn randomly)
 * @param[in] replace Defines if selection is being done with/without replacement
 * @return A vector of individuals (parents)
 *
 * @note The user can define his or her own selection operators or can use any
 * other operator defined in the file selection.cpp.
 *
 * @see ktournament_selection(), roulette_wheel_selection(),
 * linear_rank_selection(), random_selection(),
 * stochastic_roulette_wheel_selection(), trunctation_selection()
 */
std::vector<individual_s> GA::selection(size_t num_parents,
                                        int k=2,
                                        bool replace=true)
{
    std::vector<individual_s> parents;
    parents = ktournament_selection(population, num_parents, k, replace);
    return parents;
}


/**
 * Applies a crossover operator on the genomes of two parents (parent1 and
 * parent2) to create a child genome.
 * By default, this method calls the one-point crossover operator.
 *
 * @param[in] parent1 Genome of the first parent
 * @param[in] parent2 Genome of the second parent
 * @return The genome of a child as a vector of type REAL_
 *  
 * @note The user can define his or her own crossover operator or they can 
 * use any mutation operator that is defined in the file crossover.cpp.
 *
 * @see one_point_crossover(), two_point_crossover(), uniform_crossover(),
 * flat_crossover(), discrete_crossover()
 */
std::vector<REAL_> GA::crossover(std::vector<REAL_> parent1,
                                 std::vector<REAL_> parent2)
{
    std::vector<REAL_> child;
    child = one_point_crossover(parent1, parent2);
    return child;
}


/**
 * The basic mutation operator. By default, this is a delta mutation operator.
 *
 * @param[in] genome The genome (genes) of an individual
 * @param[in] mutation_rate Mutation rate (probability threshold)
 * @param[in] variance  Standard deviation of the normal distribution from which the
 * increment/decrement is drawn
 * @return A vector of floats (mutated genome)
 *
 * @note The used can define his or her own method of mutation or they can use
 * any other operator defined in the file mutation.cpp.
 *
 * @see delta_mutation(), random_mutation(), nonuniform_mutation(), fusion_mutation()
 */
std::vector<REAL_> GA::mutation(std::vector<REAL_> genome,
                                REAL_ mutation_rate=0.5,
                                REAL_ variance=0.5)
{
    std::vector<REAL_> tmp;
    tmp = delta_mutation(genome, mutation_rate, variance);
    return tmp;
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
        parents = selection(2, 2, false);
        parent1 = parents[0];
        parent2 = parents[1];
        /* parent1 = parents[i]; */
        /* parent2 = parents[i+1]; */
       
        // Reset the selection flags 
        // This has to be placed outside the loop in case the selection method
        // is placed outside the loop
        reset_selection_flags();

        // Crossover
        child = crossover(parent1.genome, parent2.genome);
        // child = order_one_crossover(parent1.genome, parent2.genome);

        // Mutation
        /* REAL_ tmp = 85*pow((1 - 1/5000), iter_index) + 0.5; */
        /* child = mutation(child, 0.5, tmp); */
        child = mutation(child);
        // child = swap_mutation(child);

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
    for(size_t i = 0; i < generations; ++i) {
        run_one_generation();
    }
    sort_population();

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
