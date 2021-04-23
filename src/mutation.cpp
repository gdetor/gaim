/* Mutation Operators cpp file for GAIM software 
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
 * @file mutation.cpp
 * Provides extra mutation operators (which produce a mutant genome from a   
 * current genome) usable by default within GAIM. Some operators should be called 
 * multiple times per generation.
 */
// $Log$
#include "gaim.h"


/**
 * This function implements a delta mutation operator meaning that for every 
 * gene in the genome a random number drawn from a normal
 * distribution and it is added to the gene value according to a probability.
 * Specifically, the mutation delta is drawn from the Normal/Gaussian distribution
 * with mean 0 and standard deviation defined by the argument variance.
 *
 * @param[in] genome The genome (genes) of an individual
 * @param[in] mutation_rate Mutation rate (probability threshold)
 * @param[in] variance  Standard deviation of the normal distribution from which the
 * increment/decrement is drawn
 * @return A vector of floats (mutated genome)
 */
std::vector<REAL_> delta_mutation(std::vector<REAL_> genome,
                                  REAL_ mutation_rate=0.5,
                                  REAL_ variance=0.5)
{
    std::vector<REAL_> tmp;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> probs(0, 1);
    static std::normal_distribution<> delta(0, variance);

    tmp = genome;
    for (size_t i = 0; i < genome.size(); ++i) {
        if (probs(gen) <= mutation_rate) {
            tmp[i] += delta(gen);
        }
    }
    return tmp;
}


/**
 * @brief Random mutation operator.
 *
 * Performs a random mutation on the genome of an individual. A random mutation
 * takes place at a random gene and replaces the current value of the gene with
 * a new random one drawn from a uniform distribution in the interval [a, b].
 *
 * @param[in] genome Individual's genome (genes)
 * @param[in] a Lower boundary of interval [a, b]
 * @param[in] b Upper boundary of interval [a, b]
 * @param[in] is_real Determines if the interval is in the set of reals or
 * integers (to use when REAL_ is set to INT)
 *
 * @return A mutated genome (vector of type REAL_). 
 */
std::vector<REAL_> random_mutation(std::vector<REAL_> genome,
                                   REAL_ a,
                                   REAL_ b,
                                   bool is_real)
{
    std::vector<REAL_> mutated_genome;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> U(0, genome.size());

    mutated_genome = genome;
    if (is_real) {
        static std::uniform_real_distribution<> R(a, b);
        mutated_genome[U(gen)] = R(gen);
    } else {
        static std::uniform_int_distribution<> R(a, b);
        mutated_genome[U(gen)] = R(gen);
    }
    return mutated_genome;
}


/**
 * @brief Auxiliary function for decaying mutation, used in time-dependent 
 * non-uniform mutation.
 *
 * Performs the following operation
 * \f[ x (1 - \lambda^{{1 - \frac{t}{t_{tot}}}^r})  \f]  [1]
 *
 * @param[in] x Gene's numerical value
 * @param[in] time Current time step (generation)
 * @param[in] tot_time Total number of time steps (generations)
 * @param[in] r Magnitude of order that the LHS of [1] affects the mutation
 *
 * @return A mutated gene value.
 */
REAL_ delta(REAL_ x, size_t time, size_t tot_time, size_t r=1)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> U(0, 1);

    REAL_ L = U(gen);
    return x * (1 - pow(L, pow(1 - time / tot_time, r)));
}


/**
 * @brief Non-uniform mutation operator.
 *
 * Performs a non-uniform mutation which is time-dependent, decaying over 
 * time. The mutation is computed as \f$ \Delta(b - x) \f$ or \f$\Delta(x - a) \f$
 * based on a random choice between -1 or 1, respectively. 
 * 
 * @param[in] genome Individual's genome
 * @param[in] time Current generation (time step)
 * @param[in] generations Total number of generations (total time steps)
 * @param[in] order Power of mutation rate
 * @param[in] a Lower boundary of interval [a, b]
 * @param[in] b Upper boundary of interval [a, b]
 *
 * @return A mutated genome.
 *
 */
std::vector<REAL_> nonuniform_mutation(std::vector<REAL_> genome,
                                       size_t time,
                                       size_t generations,
                                       size_t order,
                                       REAL_ a,
                                       REAL_ b)
{
    std::vector<REAL_> mutated_genome;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> U(-1, 1);

    mutated_genome = genome;
    size_t sign = U(gen);
    if (sign == 1) {
        for (auto &g : mutated_genome) {
            g += delta(b - g, time, generations, order);
        }
    } else {
        for (auto &g : mutated_genome) {
            g -= delta(g - a, time, generations, order);
        }
    }
    return mutated_genome;
}


/**
 * @brief Fusion mutation operator method.
 *
 * Performs a fusion mutation operation. It checks if the genome has unique
 * elements. If doesn't have, it removes the repetitions and replaces them by
 * new random (uniformly drawn) values. 
 *
 * @param[in] genome Individual's genome
 * @param[in] a Lower bound of sampling interval
 * @param[in] b Upper bound of sampling interval
 * @param[in] is_real Determines if the interval is in the set of reals or
 * integers (to use when REAL_ is set to INT)
 *
 * @return A mutated genome.
 */
std::vector<REAL_> fusion_mutation(std::vector<REAL_> genome,
                                   REAL_ a,
                                   REAL_ b,
                                   bool is_real)
{
    std::vector<REAL_> mutated_genome;
    std::vector<REAL_>::iterator it;
    static std::random_device rd;
    static std::mt19937 gen(rd());

    mutated_genome = genome;
    std::set<REAL_> unique_genome;
    if (is_real) {
        static std::uniform_real_distribution<> R(a, b);
        for (size_t i = 0; i < mutated_genome.size(); ++i) {
            if (unique_genome.find(mutated_genome[i]) != unique_genome.end()) {
                mutated_genome[i] = R(gen);
            } 
            unique_genome.insert(mutated_genome[i]);
        }
    } else {
        static std::uniform_int_distribution<> R(a, b);
        for (size_t i = 0; i < mutated_genome.size(); ++i) {
            if (unique_genome.find(mutated_genome[i]) != unique_genome.end()) {
                mutated_genome[i] = R(gen);
            } 
            unique_genome.insert(mutated_genome[i]);
        }
    }
    return mutated_genome;
}


/**
 * @brief Swap mutation operator method.
 *
 * Performs a swap mutation operation. It randomly picks up two genes on the 
 * same genome and swaps them. 
 * 
 * @note The swap mutation operator is useful in cases of permutations (for
 * instance in solving problems such as the Traveling Salesman Problem).
 *
 * @param[in] genome Individual's genome
 *
 * @return A mutated genome.
 */
std::vector<REAL_> swap_mutation(std::vector<REAL_> genome)
{
    size_t idx_a(0), idx_b(0);
    std::vector<REAL_> mutated_genome;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> R(0, genome.size()-1);

    mutated_genome = genome;
    idx_a = R(gen);
    idx_b = R(gen);
    while (idx_a == idx_b) {
        idx_b = R(gen);
    }
    std::swap(mutated_genome[idx_a], mutated_genome[idx_b]);
    return mutated_genome;   
}
