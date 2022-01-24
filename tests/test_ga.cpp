/*
 * Test cases for GA
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
#include "gaim.h"


REAL_ square(REAL_ x) {
    return x*x;
}


ga_parameter_s init_ga_params(void)
{
    std::vector<REAL_> a(2, -1.0);
    std::vector<REAL_> b(2, 1.0);

    mut_parameter_s mut;
    cross_parameter_s cross;
    sel_parameter_s sel;
    ga_parameter_s ga_test;

    sel.selection_method = "ktournamet";
    sel.bias = 1.5;
    sel.num_parents = 2;
    sel.lower_bound = 1;
    sel.k = 2;
    sel.replace = false;

    cross.crossover_method = "one_point";

    mut.mutation_method = "delta";
    mut.mutation_rate = 0.5; 
    mut.variance = 0.5; 
    mut.low_bound = 0.0; 
    mut.up_bound = 1.0; 
    mut.time = 1;   
    mut.order = 1;  
    mut.is_real = true;  

    ga_test.sel_pms = sel;
    ga_test.cross_pms = cross;
    ga_test.mut_pms = mut;
    ga_test.a = a;
    ga_test.b = b;
    ga_test.generations = 1000;
    ga_test.population_size = 10;
    ga_test.genome_size = 2;
    ga_test.num_offsprings = 1;
    ga_test.num_replacement = 1;
    ga_test.runs = 1;
    ga_test.clipping = "universal";
    ga_test.clipping_fname = "test";
    return ga_test;
}


pr_parameter_s init_print_params(void) 
{
    pr_parameter_s pr_test;
    pr_test.where2write = "stdout";
    pr_test.experiment_name = "test_";
    pr_test.print_fitness = false;
    pr_test.print_average_fitness = false;
    pr_test.print_bsf = false;
    pr_test.print_best_genome = false;
    return pr_test;
}


int test_evaluation(std::size_t population_size)
{
    std::size_t count = 0;
    REAL_ ground_truth = 0;
    std::vector<REAL_> genome_(2, 2);
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = 2;
    pms.population_size = population_size;

    GA test(&pms);

    for (std::size_t i = 0; i < population_size; ++i) {
        test.population[i].genome = genome_;
    }

    test.evaluation(test.population);

    for (auto &i : genome_) {
        ground_truth += i * i;
    }

    for (std::size_t i = 0; i < population_size; ++i) {
        if (-test.population[i].fitness == ground_truth) {
            count++;
        }
    }

    if(count == population_size) {
        return 0;
    } else {
        return -1;
    }
}


int test_sort_population(std::size_t population_size)
{
    size_t count = 0;
    std::vector<REAL_> order(population_size);
    std::iota(order.begin(), order.end(), 0);
    std::random_shuffle(order.begin(), order.end());
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = 2;
    pms.population_size = population_size;

    GA test(&pms);

    for (std::size_t i = 0; i < population_size; ++i) {
        test.population[i].fitness = order[i];
    }

    test.sort_population();

    for (std::size_t i = 0; i < population_size; ++i) {
        if (test.sorted_population[i].fitness == (REAL_) i) {
            count++;
        }
    }

    if (count == population_size) {
        return 0;
    } else {
        return -1;
    }
}


int test_next_generation(std::size_t population_size,
                         std::size_t num_offsprings,
                         std::size_t perc)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> values_p(5, 40);
    std::uniform_real_distribution<> values_c(50, 100);
    ga_parameter_s pms(init_ga_params());
    
    pms.genome_size = 2;
    pms.population_size = population_size;
    pms.num_offsprings = num_offsprings;
    pms.num_replacement = perc;

    GA test(&pms);

    std::vector<REAL_> genomes_p(10, 4), genomes_c(10, -6);

    for (std::size_t i = 0; i < population_size; ++i) {
        test.population[i].fitness = values_p(gen);
        test.population[i].genome = genomes_p;
    }

    for (std::size_t i = 0; i < num_offsprings; ++i) {
        test.offsprings[i].fitness = values_c(gen);
        test.offsprings[i].genome = genomes_c;
    }

    test.next_generation(perc);

    size_t count = 0;
    std::sort(test.population.begin(), test.population.end(), compare_fitness);
    for (std::size_t i = 0; i < perc; ++i) {
        if (test.population[i].genome == test.offsprings[i].genome) {
            ++count;
        }
    }
    if (count == population_size) {
        return -1;
    }
    return 0;
}


int test_run_one_generation(std::size_t generations,
                            std::size_t population_size)
{
    std::size_t dist;
    float res;
    std::vector<int> index;
    std::vector<int>::iterator it;
    ga_parameter_s pms(init_ga_params());
    pr_parameter_s pr_pms(init_print_params());
    
    pms.genome_size = 2;
    pms.population_size = population_size;
    pms.num_offsprings = 5;
    pms.num_replacement = 3;

    GA gen_alg(&pms);
    gen_alg.evolve(generations, 0, &pr_pms);

    for (auto &p : gen_alg.population) {
        res += p.fitness;
        index.push_back(p.id);
    }

    res /= gen_alg.population.size();
    it = std::unique(index.begin(), index.end());

    dist = std::distance(index.begin(), it);

    if (res >= 0.1 && dist != gen_alg.population.size()) {
        return 1;
    }
    return 0;
}


int main() {
    // Testing evaluation of fitness
    int id = 0;
    std::cout << "Testing evaluation of fitness (x3)." << std::endl;
    id = test_evaluation(10);
    cross_validate_(id, "Evaluation");
    id = test_evaluation(30);
    cross_validate_(id, "Evaluation");
    id = test_evaluation(5);
    cross_validate_(id, "Evaluation");

    // Testing population sorting
    std::cout << "Testing population sorting (x3)." << std::endl;
    id = test_sort_population(10);
    cross_validate_(id, "Sorting");
    id = test_sort_population(30);
    cross_validate_(id, "Sorting");
    id = test_sort_population(5);
    cross_validate_(id, "Sorting");

    // Testing next generation 
    std::cout << "Testing next generation method (x3)." << std::endl;
    id = test_next_generation(10, 5, 2);
    cross_validate_(id, "Offsprings");
    id = test_next_generation(50, 30, 5);
    cross_validate_(id, "Offsprings");
    id = test_next_generation(150, 70, 35);
    cross_validate_(id, "Offsprings");
        
    // Testing iteration over generations
    std::cout << "Testing entire GA evolving process (x3)." << std::endl;
    id = test_run_one_generation(100, 10);
    cross_validate_(id, "Evolving process");
    id = test_run_one_generation(1000, 50);
    cross_validate_(id, "Evolving process");
    id = test_run_one_generation(5000, 5);
    cross_validate_(id, "Evolving process");
    return 0;
}
