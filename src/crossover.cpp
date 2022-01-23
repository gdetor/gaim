/* Crossover Operators cpp file for GAIM software 
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
 * @file crossover.cpp
 * Implements extra crossover operators, which specify
 * how genes from two parent genomes will be combined to produce a child genome. 
 * Where applicable, one of two possible children will result with equal probability.
 * 
 * The methods exit fatally if the size of genomes of the two parents is unequal.
 */
// $Log$
#include "gaim.h"


/**
 * @brief Assigns the appropriate crossover operator to the pointer function
 * *crossover.
 *
 * This method assigns to the crossover operator (pointer function) the
 * appropriate method based on the crossover method string. The crossover method
 * is provided by the data structure cross_parameter_s.
 *
 * @param[in] void
 * @return Nothing
 *
 * @see cross_parameter_s
 */
void GA::select_crossover_method(void) {
    if (crossover_method == "one_point") {
        crossover = &GA::one_point_crossover;
    } else if (crossover_method == "two_point") {
        crossover = &GA::two_point_crossover;
    } else if (crossover_method == "uniform") {
        crossover = &GA::uniform_crossover;
    } else if (crossover_method == "flat") {
        crossover = &GA::flat_crossover;
    } else if (crossover_method == "discrete") {
        crossover = &GA::discrete_crossover;
    } else if (crossover_method == "first_order") {
        crossover = &GA::order_one_crossover;
    } else {
        std::cout << "Error: GA Crossover method not found!" << std::endl;
        exit(-1);
    }
}


/**
 * Applies a crossover operator on the genomes of two parents (parent1 and
 * parent2) to create a child genome.
 * By default, this is a one-point crossover operator.
 *
 * @param[in] parent1 Genome of the first parent
 * @param[in] parent2 Genome of the second parent
 * @return The genome of a child as a vector of type REAL_
 */
std::vector<REAL_> GA::one_point_crossover(std::vector<REAL_> parent1,
                                           std::vector<REAL_> parent2)
{
    int order;
    size_t xover;
    std::vector<REAL_> child1, child2;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> uniform_num(0, parent1.size()-1);
    static std::uniform_int_distribution<> choice(0, 1);

    xover = uniform_num(gen);
    order = choice(gen);
    
    child1 = parent1;
    child2 = parent2;
    for (size_t i = xover; i < child1.size(); ++i) {
        child1[i] = parent2[i];
        child2[i] = parent1[i];
    } 
    
    if (order == 0) {
        return child1;
    } else {
        return child2;
    }
}


/**
 * @brief Two-point crossover operator
 *
 * Performs a two-point crossover. Two random points on the genome 
 * are drawn from a uniform distribution and the genes within the  
 * area defined by the two points is swapped between parents. 
 *
 * @param[in] parent1 First parent's genome
 * @param[in] parent2 Second parent's genome
 *
 * @return Offspring's genome 
 */
std::vector<REAL_> GA::two_point_crossover(std::vector<REAL_> parent1,
                                           std::vector<REAL_> parent2)
{
    std::vector<REAL_> child1, child2;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> uniform_num(0, parent1.size()-1);
    static std::uniform_int_distribution<> choice(0, 1);

    size_t order = choice(gen);
    size_t xover_pt1, xover_pt2;
    size_t r1 = uniform_num(gen);
    size_t r2 = uniform_num(gen);

    xover_pt1 = std::min(r1, r2);
    xover_pt2 = std::max(r1, r2);

    if (parent1.size() != parent2.size()) {
        std::cerr << "Parents genome size mismatch!" << std::endl;
        exit(-1);
    }

    child1 = parent1;
    child2 = parent2;
    for (size_t i = xover_pt1; i < xover_pt2; ++i) {
        child1[i] = parent2[i];
        child2[i] = parent1[i];
    } 
    
    if (order == 0) {
        return child1;
    } else {
        return child2;
    }
}


/**
 * @brief Uniform crossover operator
 *
 * Performs a uniform crossover operation. Each gene is chosen from one 
 * parent or the other with equal probability. 
 *
 * @param[in] parent1 First parent's genome
 * @param[in] parent2 Second parent's genome
 *
 * @return Offspring's genome 
 */
std::vector<REAL_> GA::uniform_crossover(std::vector<REAL_> parent1,
                                         std::vector<REAL_> parent2)
{
    std::vector<REAL_> child1, child2;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> choice(0, 1);
    size_t order = choice(gen);

    if (parent1.size() != parent2.size()) {
        std::cerr << "Parents genome size mismatch!" << std::endl;
        exit(-1);
    }

    child1 = parent1;
    child2 = parent2;
    for (size_t i = 0; i < parent1.size(); ++i) {
        if (choice(gen)) { child1[i] = parent2[i]; }
        if (choice(gen)) { child2[i] = parent1[i]; }
    } 

    if (order == 0) {
        return child1;
    } else {
        return child2;
    }
}


/**
 * @brief Flat crossover operator
 *
 * Performs a "flat" crossover operation. Genes from both parent are combined in 
 * as by linear mixing, similar to alpha blending. A random variate is chosen to specify  
 * the mixing weight.
 *
 * @param[in] parent1 First parent's genome
 * @param[in] parent2 Second parent's genome
 *
 * @return Offspring's genome 
 */
std::vector<REAL_> GA::flat_crossover(std::vector<REAL_> parent1,
                                      std::vector<REAL_> parent2)
{
    std::vector<REAL_> child;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> U(0, 1);
    REAL_ R;

    if (parent1.size() != parent2.size()) {
        std::cerr << "Parents genome size mismatch!" << std::endl;
        exit(-1);
    }
    
    for (size_t i = 0; i < parent1.size(); ++i) {
        R = U(gen);
        child.push_back(R * parent1[i] + (1 - R) * parent2[i]);
    }
    return child;
}


/**
 * @brief Discrete crossover operator
 *
 * Performs a discrete crossover operation. Genes from both parent are combined
 * in a random way, slightly different from uniform crossover. The second parent's
 * genes replace genes from the first parent's genome in a random way. 
 * For each gene a random number {0, 1} is drawn and defines the replacement.
 * In the case of a 0 being drawn, the second parent's gene replaces the first parent's gene. 
 *
 * @param[in] parent1 First parent's genome
 * @param[in] parent2 Second parent's genome
 *
 * @return Offspring's genome 
 */
std::vector<REAL_> GA::discrete_crossover(std::vector<REAL_> parent1,
                                          std::vector<REAL_> parent2)
{
    std::vector<REAL_> child;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> U(0, 1);
    size_t idx; 

    if (parent1.size() != parent2.size()) {
        std::cerr << "Parents genome size mismatch!" << std::endl;
        exit(-1);
    }

    child = parent1;
    for (size_t i = 0; i < parent1.size(); ++i) {
        idx = U(gen);
        if (idx == 0) {
            child[i] = parent2[i];
        }
    }
    return child;
}


/**
 * @brief Order-one crossover operator
 *
 * Performs an order-one crossover operation. It starts from the half genome
 * of the first parent and as long as the genes between the two parents 
 * are different, it copies parent 2's genes to the child's genome. 
 *
 * @note This operator is useful in problems involving permuations. The rule is somewhat different from 
 * traditional single-point crossover, which has no guarantees of preserving parent A's half genome.
 *
 * @param[in] parent1 First parent's genome
 * @param[in] parent2 Second parent's genome
 *
 * @return Offspring's genome 
 */
std::vector<REAL_> GA::order_one_crossover(std::vector<REAL_> parent1,
                                           std::vector<REAL_> parent2)
{
    size_t len = static_cast<size_t>(parent1.size() / 2);
    std::vector<REAL_> child1(parent1.size(), 0);

    if (parent1.size() != parent2.size()) {
        std::cerr << "Parents genome size mismatch!" << std::endl;
        exit(-1);
    }

    child1 = parent1;

    for (size_t i = len; i < parent1.size()-1; ++i) {
        while (std::find(child1.begin(), child1.end(), parent1[i]) == child1.end()) {
            child1[i] = parent2[i];
        }
    }
    return child1;
}
