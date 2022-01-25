/* Selection Operators cpp file for GAIM software 
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
 * @file selection.cpp
 * Provides extra selection operators (which identify reproducing pairs to create
 * the next generation) usable by default within GAIM.
 */
// $Log$
#include "gaim.h"


/**
 * @brief Assigns the appropriate selection operator to the pointer function
 * *selection.
 *
 * This method assigns to the selection operator (pointer function) the
 * appropriate method based on the selection method string. The selection method
 * is provided by the data structure sel_parameter_s.
 *
 * @param[in] void
 * @return Nothing
 *
 * @see sel_parameter_s
 */
void GA::select_selection_method(void) {
    // Assign the appropriate selection method
    if (selection_method == "ktournament") {
        selection = &GA::ktournament_selection;
    } else if (selection_method == "truncation") {
        selection = &GA::truncation_selection;
    } else if (selection_method == "linear_rank") {
        selection = &GA::linear_rank_selection;
    } else if (selection_method == "random") {
        selection = &GA::random_selection;
    } else if (selection_method == "roulette") {
        selection = &GA::roulette_wheel_selection;
    } else if (selection_method == "stochastic_roulette") {
        selection = &GA::stochastic_roulette_wheel_selection;
    } else if (selection_method == "whitley") {
        selection = &GA::whitley_selection;
    } else {
        std::cout << "Error: GA Selection method not found!" << std::endl;
        exit(-1);
    }
}


/**
 * Implements a k-tournament selection. 
 *
 * @param[in] population A vector of the entire population of individuals.
 * @return A vector of individual_s structures (selected parents).
 */
std::vector<individual_s> GA::ktournament_selection(std::vector<individual_s> &population) {
    int best_index = 0;
    std::vector<individual_s> selected_individuals;
    individual_s ind, best; 
    std::vector<size_t> indices(population.size());
    
    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: KTournament Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    // Create a vector of indices [0, population.size())
    std::iota(indices.begin(), indices.end(), 0);

    // Select the individuals from the population without replacement
    if (replace == false) {
        for (size_t i = 0; i < num_parents; ++i) {
            std::random_shuffle(std::begin(indices), std::end(indices));
            for (int j = 0; j < k; ++j) {
                ind = population[indices[j]];
                if ((best.genome.size() == 0) ||
                    (ind.fitness > best.fitness)) {
                        best = ind;
                        best_index = j;
                }
            }
            selected_individuals.push_back(best);
            population[indices[best_index]].is_selected = true;
            remove_at(indices, best_index);
        }
    // Select the individuals from the population with replacement
    } else {
        for (size_t i = 0; i < num_parents; ++i) {
            for (int j = 0; j < k; ++j) {
                ind = population[indices[j]];
                if ((best.genome.size() == 0) ||
                    (ind.fitness > best.fitness)) {
                        best = ind;
                }
            }
            selected_individuals.push_back(best);
        }
    }
    return selected_individuals;
}


/**
 * Implements a truncation selection operator. The population is sorted based on 
 * the fitness of each individual and a percentage of individuals that will be 
 * discarded is computed based on the given number of offsprings (lambda). 
 *
 * @param[in] population A vector of the entire population of individuals
 * @return A vector of individual_s structures (or the selected parents). 
 */
std::vector<individual_s> GA::truncation_selection(std::vector<individual_s> &population) {
    size_t r = 0, mu = population.size()-1;
    std::vector<size_t> indices(num_parents);
    std::vector<individual_s> selected_individuals;

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: RW Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    if ((lower_bound > num_parents) || (lower_bound > population.size())) {
        std::cerr << "Truncation Selection: Lower bound wrong size!" << std::endl;
        exit(-1);
    }

    // Sort the individuals based on their fitness - Create the rank
    std::sort(population.begin(), population.end(), compare_fitness);

    // Select the individuals from the population without replacement
    if (replace == false) {
        std::iota(indices.begin(), indices.end(), lower_bound);
        std::random_shuffle(std::begin(indices), std::end(indices));
        for(size_t i = 0; i < num_parents; ++i) {
            selected_individuals.push_back(population[indices[i]]);
            population[indices[i]].is_selected = true;
        }
    // Select the individuals from the population with replacement
    } else {
        for(size_t i = 0; i < num_parents; ++i) {
            r = int_random(lower_bound, mu);
            selected_individuals.push_back(population[r]);
        }
    }
    return selected_individuals;
}


/**
 * Implements a linear rank selection mechanism. The population is sorted
 * based on the fitness of each individual and then a selection probability
 * is assigned to each individual based on their rank. 
 *
 * @param[in] population A vector of the entire population of individuals
 * replacement or not.
 * @return A vector of individual_s structures (or the selected parents). 
 */
std::vector<individual_s> GA::linear_rank_selection(std::vector<individual_s> &population) {
    size_t index = 0;
    REAL_ prob = 0;
    std::vector<individual_s> selected_individuals;
    individual_s best;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> uniform(0, 1);

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: Rank Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    // Sort the individuals based on their fitness - Create the rank
    std::sort(population.begin(), population.end(), compare_fitness);

    // Select the individuals from the population without replacement
    if (replace == false) {
        while(selected_individuals.size() != num_parents) {
            REAL_ r = uniform(gen);
            for (size_t i = 0; i < population.size(); ++i) {
                prob += (REAL_) (i+1) / population.size();
                if ((r < prob) && (population[i].is_selected == false)) {
                    index = i;
                    break;
                }
            }
            selected_individuals.push_back(population[index]);
            population[index].is_selected = true;
            prob = 0;
        }
    // Select the individuals from the population with replacement
    } else {
        for (size_t k = 0; k < num_parents; ++k) {
            REAL_ r = uniform(gen);
            for (size_t i = 0; i < population.size(); ++i) {
                prob += (REAL_) (i+1) / population.size();
                if (r < prob) {
                    best = population[i];
                    break;
                }
            }
            selected_individuals.push_back(best);
            prob = 0;
        }
    }
    return selected_individuals;
}


/**
 * Implements a random selection. It randomly picks up one individual from
 * the entire population.
 *
 * @param[in] population A vector of the entire population of individuals.
 * @return A vector of indivudual_s as parents. 
 */
std::vector<individual_s> GA::random_selection(std::vector<individual_s> &population) {
    size_t idx;
    std::vector<size_t> indices(population.size());
    std::vector<individual_s> selected_individuals;

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: Random Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    // Select the individuals from the population without replacement
    if (replace == false) {
        std::iota(indices.begin(), indices.end(), 0);
        std::random_shuffle(std::begin(indices), std::end(indices));
        for (size_t i = 0; i < num_parents; ++i) {
            selected_individuals.push_back(population[indices[i]]);
            population[indices[i]].is_selected = true;
        }
    // Select the individuals from the population with replacement
    } else {
        for(size_t i = 0; i < num_parents; ++i) {
            idx = int_random(0, population.size()-1);
            selected_individuals.push_back(population[idx]);
        }
    }
    return selected_individuals;
}


/**
 * Implements a roulette-wheel selection (or fitness-proportionate selection)
 * mechanism. The fitness function observes a fitness for each individual 
 * and the share of the population's total fitness is used to associate a selection  
 * probability with each individual. 
 *
 * @param[in] population A vector of the entire population of individuals.
 * @return A vector of individual_s which corresponds to the selected parents. 
 */
std::vector<individual_s> GA::roulette_wheel_selection(std::vector<individual_s> &population) {
    size_t index = 0;
    REAL_ prob = 0, r = 0;
    std::vector<individual_s> selected_individuals;

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: RW Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    // Compute the cumulative fitness of the entire population
    static REAL_ cumulative = std::accumulate(population.begin(),
                                              population.end(),
                                              0,
                                              average_fitness);

    // Select the individuals from the population without replacement
    if (replace == false) {
        for(size_t i = 0; i < num_parents; ++i) {
            prob = 0;
            r = float_random(0, 1);
            for (size_t j = 0; j < population.size(); ++j) {
                prob += population[j].fitness / cumulative;
                if ((r < prob) && (population[j].is_selected == false)) {
                    index = j;
                    break;
                }
            }
            selected_individuals.push_back(population[index]);
            population[index].is_selected = true;
            cumulative = cumulative_fitness(population);
        }
    // Select the individuals from the population with replacement
    } else {
        for(size_t i = 0; i < num_parents; ++i) {
            prob = 0;
            r = float_random(0, 1);
            for (size_t j = 0; j < population.size(); ++j) {
                prob += population[j].fitness / cumulative;
                if (r < prob) {
                    index = j;
                    break;
                }
            }
            selected_individuals.push_back(population[index]);
        }
    }
    return selected_individuals;
}


/**
 * Parent selection method proposed by Lipowski A. and Lipowska D. in 
 * "Roulette wheel selection via stochastic acceptance", 2011. The method
 * is a variant of the roulette wheel where the maximum fitness is used to
 * normalize individuals' fitness and transform them to a probability. The
 * latter is used to decide probabilisticly if the chosen individual will be
 * allowed to get offsprings.
 *
 * @param[in] population A vector of the entire population of individuals.
 * @return A vector of indivudual_s (selected parents). 
 */
std::vector<individual_s> GA::stochastic_roulette_wheel_selection(std::vector<individual_s> &population) {
    size_t index(0), idx(0);
    REAL_ prob(0), max_fitness(1), r(0);
    std::vector<individual_s> selected_individuals;

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: RW Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }
            
    // Select the individuals from the population without replacement
    if (replace == false) {
        max_fitness = maximum_fitness(population);
        std::vector<size_t> indices(population.size());
        std::iota(indices.begin(), indices.end(), 0);
        for(size_t i = 0; i < num_parents; ++i) {
            r = float_random(0, 1);
            index = indices[int_random(0, indices.size()-1)];
            prob = population[index].fitness / max_fitness;
            if ((prob > r) && (population[index].is_selected == false)) {
                selected_individuals.push_back(population[index]);
                population[index].is_selected = true;
                remove_at(indices, index);
            }
            max_fitness = nonselected_maximum_fitness(population);
        }
    // Select the individuals from the population with replacement
    } else {
        for(size_t i = 0; i < num_parents; ++i) {
            idx = int_random(0, population.size() - 1);
            prob = population[idx].fitness / max_fitness;
            r = float_random(0, 1);
            if (prob > r) {
                index = idx;
            }
            selected_individuals.push_back(population[index]);
        }
    } 
    return selected_individuals;
}


/**
 * Calculates the Whitley factor used in Whitley's selection method.
 *
 * @param[in] bias Governs how strong one would like to favor high ranked
 * individuals.
 * @return The factor bias - sqrt(bias^2 - 4 * (bias - 1) * rand(0, 1))/2/(bias-1)
 *
 * @warning Bias should be in (1, 2]
 *
 * @see whitley_selection()
 */
REAL_ calculate_whitley_factor(REAL_ bias) {
    REAL_ tmp;
    REAL_ R = float_random(0, 1);
    tmp = (bias - sqrt(bias*bias - 4.0*(bias-1) * R)) / 2.0 / (bias-1);
    return tmp;
}


/**
 * Parent selection method proposed by Darrell Whitley in his GENITOR paper:
 * "The GENITOR Algorithm and Selection Pressure: Why Rank-Based Allocation
 * of Reproductive Trials is Best", ", D. Whitley, 2000. 
 *
 * @param[in] population A vector of the entire population of individuals.
 * @return A vector of indivudual_s (selected parents). 
 *
 * @see calculate_whitley_factor() On how to calculate the Whitley factor.
 */
std::vector<individual_s> GA::whitley_selection(std::vector<individual_s> &population) {
    size_t best = 0;
    REAL_ wt_factor = 0.0;
    size_t index;
    std::vector<individual_s> selected_individuals;

    // Check if the number of parents is greater than the population size
    if (population.size() < num_parents) {
        std::cout << "WARNING: Random Selection Method" << std::endl;
        std::cout << "Parents size > Population Size" << std::endl;
        std::cout << "Parents size reduced to population size" << std::endl;
        num_parents = population.size();
    }

    // If number of parents is the same as the population size (generational
    // replacement)
    if (num_parents == population.size()) {
        selected_individuals = population;
        return selected_individuals;
    }

    // If bias is set to 1, correct it
    if (bias == 1.0) { bias = 1.5; }

    // Select the individuals from the population without replacement
    if (replace == false) {
        for(size_t i = 0; i < num_parents; ++i) {
            wt_factor = calculate_whitley_factor(bias);
            index = static_cast<size_t>(population.size() * wt_factor);
            if (population[index].is_selected == false) {
                best = index;
            }
            selected_individuals.push_back(population[best]);
            population[best].is_selected = true;
        }
    // Select the individuals from the population with replacement
    } else {
        for(size_t i = 0; i < num_parents; ++i) {
            wt_factor = calculate_whitley_factor(bias);
            index = (size_t) (population.size() * wt_factor);
            selected_individuals.push_back(population[index]);
            
        }
    }
    return selected_individuals;
}
