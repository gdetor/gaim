/* Auxiliary Functions cpp file for GAIM software 
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
 * @file auxiliary_funs.cpp
 * Auxiliary functions for the GAIM package, encompassing helpers for 
 * arithmetic with individual fitnesses and wrappers for directory I/O.
 */
// $Log$

#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <cstdint>
#include "gaim.h"


/**
 * Integer uniform distribution. It returns an integer random number in the
 * interval [a, b] (uniform distribution).
 *
 * @param[in] a Lower limit of range [a, b] from which a random integer will be
 * drawn.
 * @param[in] b Upper limit of range [a, b].
 * @return An integer number in [a, b].
 */
size_t int_random(size_t a, size_t b) {
    static pcg_extras::seed_seq_from<std::random_device> seed_source;
    static pcg32 rng(seed_source);
    static std::uniform_int_distribution<size_t>  distr(a, b);
    return distr(rng);
}


/**
 * Float uniform distribution. It returns a float random number in the
 * interval [a, b] (uniform distribution).
 *
 * @param[in] a Lower limit of range [a, b] from which a random float will be
 * drawn.
 * @param[in] b Upper limit of range [a, b].
 * @return A float number in [a, b].
 */
REAL_ float_random(REAL_ a, REAL_ b) {
    static pcg_extras::seed_seq_from<std::random_device> seed_source;
    static pcg32 rng(seed_source);
    static std::uniform_real_distribution<REAL_>  distr(a, b);
    return distr(rng);
}


/**
 * Removes an element from a vector by swaping the element at position n with 
 * the last one and poping it out of the vector.
 *
 * @param[in] v The input vector from whith an element will be deleted.
 * @param[in] n The size_type of the vector (position of deleted element).
 * @return (void)
 */
void remove_at(std::vector<size_t> &v, std::vector<size_t>::size_type n)
{
    std::swap(v[n], v.back());
    v.pop_back();
}


/**
 * Computes the cumulative fitness of a population based on which individuals
 * have not been yet selected. This function is used in the
 * roulette_wheel_selection with replacement.
 *
 * @param[in] population A vector of the entire population.
 * @return The cumulative fitness of a population.
 *
 * @see roulette_wheel_selection()
 */
REAL_ cumulative_fitness(std::vector<individual_s> population) {
    REAL_ cumulative_fit = 0.0;

    for (auto &ind : population) {
        if (ind.is_selected == false) {
            cumulative_fit += ind.fitness;
        }
    }
    return cumulative_fit;
}


/**
 * A naive implementation of maximum. This function finds the maximum fitness
 * and returns it.
 *
 * @param[in] population A vector of the entire population.
 * @return The maximum fitness of a population.
 */
REAL_ maximum_fitness(std::vector<individual_s> population)
{
    REAL_ max_fitness(0);

    max_fitness = population[0].fitness;
    for (size_t i = 0; i < population.size(); ++i) {
        if (population[i].fitness > max_fitness) {
            max_fitness = population[i].fitness;
        }
    }
    return max_fitness;
}


/**
 * Computes the maximum fitness of all the non-selected (from a selection
 * operator) indivuduals of a population.
 *
 * @note This functions is meant to be used with selection methods that use the
 * maximum fitness of a population and have the selection-with-replacement
 * option disabled.
 * 
 * @param[in] population A vector of the entire population.
 * @return The maximum fitness of a population (of the non-selected
 * individuals).
 */
REAL_ nonselected_maximum_fitness(std::vector<individual_s> population)
{
    REAL_ max_fitness(0);

    max_fitness = population[0].fitness;
    for (size_t i = 0; i < population.size(); ++i) {
        if ((population[i].fitness > max_fitness) &&
            (population[i].is_selected == false)) {
            max_fitness = population[i].fitness;
        }
    }
    return max_fitness;
}


/**
 * This helper function compares the fitness of two individuals and 
 * returns true if the first argument has a smaller fitness or false otherwise.
 *
 * @param[in] x Data struct of type const individual_s.
 * @param[in] y Data struct of type const individual_s.
 * @return True if x.fitness < y.fitness, false otherwise.
 */
bool compare_fitness(const individual_s x, const individual_s y)
{
    return (x.fitness < y.fitness);
}


/**
 * This is a helper function serving as an binary operator for std:accumulate.
 * It helps add up the fitness belonging to individuals so that the cumulative 
 * fitness in the population can be computed, which is necessary for
 * roulette-wheel selection, etc.
 *
 * @see one_generation_step()
 *
 * @param[in] x REAL_, it is the current accumulated fitness.
 * @param[in] y Data struct of individual_s type, it provides the new fitness 
 *          of an idividual.
 * @return The current accumulated fitness yielded in the accumulation (usually
 * for the whole population.)
 */
REAL_ average_fitness(REAL_ x, const individual_s y)
{
    return x + y.fitness;
}


/**
 * Checks if the given path (directory or file) exists. 
 *
 * @param str const string that contains the name of the path.
 * @return True if the path exists and false otherwise.
 */
bool is_path_exist(const std::string &str)
{
    struct stat buffer;
    return (stat(str.c_str(), &buffer) == 0);
}


/**
 * Deletes a file.
 *
 * @param[in] fname const string that contains the name of the file to being
 * deleted. 
 * @return Zero in the case of successful deletion, non-zero value otherwise.
 */
int remove_file(const std::string fname)
{
    return remove(fname.c_str());
}


/**
 * Attempts to make a directory.
 *
 * @param[in] path const string that contains the name (path) of the new
 *             directory. 
 *
 * @return Zero upon successfully created directory, one otherwise.
 */
int mkdir_(const std::string &path)
{
    if (mkdir(path.c_str(), 0777) == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return 1;
    } else {
        std::cout << "Directory " << path << " created" << std::endl;
        return 0;
    }
}
