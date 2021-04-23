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
    ga_parameter_s ga_test;
    ga_test.a = -1;
    ga_test.b = 1;
    ga_test.generations = 1000;
    ga_test.population_size = 1;
    ga_test.genome_size = 2;
    ga_test.num_offsprings = 1;
    ga_test.num_replacement = 1;
    ga_test.runs = 1;
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


int test_evaluation(std::size_t gene_size, std::size_t population_size)
{
    std::size_t count = 0;
    REAL_ ground_truth = 0;
    std::vector<REAL_> genome_(gene_size, 2);
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = gene_size;
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


int test_sort_population(std::size_t gene_size, std::size_t population_size)
{
    size_t count = 0;
    std::vector<REAL_> order(population_size);
    std::iota(order.begin(), order.end(), 0);
    std::random_shuffle(order.begin(), order.end());
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = gene_size;
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


int test_selection(std::size_t gene_size, std::size_t population_size)
{
    bool flag = false;
    individual_s parent;
    std::pair<individual_s, individual_s> parents;
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = gene_size; 
    pms.population_size = population_size;

    GA test(&pms); 
    parents = test.selection(2);
    parent = parents.first;

    if (parent.id >=0 && parent.id < population_size) {
        flag = true;
    } else {
        flag = false;
    }

    if ((parent.genome == test.population[parent.id].genome) && flag == true) {
        return 0;
    } else {
        return -1;
    }
}


int test_crossover(std::size_t gene_size)
{
    std::vector<REAL_> parent1(gene_size, 3), parent2(gene_size, 13);
    std::vector<REAL_> child(gene_size, 0);
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = gene_size; 
    GA test(&pms);

    child = test.crossover(parent1, parent2);
    std::size_t count1 = 0, count2 = 0;
    for (std::size_t i = 0; i < child.size(); ++i) {
        if (child[i] == parent2[i]) {
            count2++;
        } else if (child[i] == parent1[i]) {
            count1++;
        }
    }
    if (count1 == 0 || count2 == 0) {
        return -1;
    } else {
        return 0;
    }
}


int test_mutation(std::size_t gene_size, REAL_ mutation_rate, REAL_ var)
{
    std::vector<REAL_> child(gene_size, 5), new_child(gene_size, 0);
    ga_parameter_s pms(init_ga_params());

    pms.genome_size = gene_size; 
    GA test(&pms);
    new_child = test.mutation(child, mutation_rate, var);

    if (child == new_child) {
        return -1;
    } else {
        return 0;
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
    
    pms.genome_size = 10;
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

    std::sort(test.population.begin(), test.population.end(), compare_fitness);
    for (std::size_t i = 0; i < perc; ++i) {
        if (test.population[i].genome != test.offsprings[i].genome) {
            return -1;
        }
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
    id = test_evaluation(1, 10);
    cross_validate_(id, "Evaluation");
    id = test_evaluation(5, 30);
    cross_validate_(id, "Evaluation");
    id = test_evaluation(50, 5);
    cross_validate_(id, "Evaluation");

    // Testing population sorting
    std::cout << "Testing population sorting (x3)." << std::endl;
    id = test_sort_population(1, 10);
    cross_validate_(id, "Sorting");
    id = test_sort_population(5, 30);
    cross_validate_(id, "Sorting");
    id = test_sort_population(50, 5);
    cross_validate_(id, "Sorting");

    // Testing selection 
    std::cout << "Testing selection method (x3)." << std::endl;
    id = test_selection(2, 10);
    cross_validate_(id, "Selection");
    id = test_selection(20, 20);
    cross_validate_(id, "Selection");
    id = test_selection(50, 10);
    cross_validate_(id, "Selection");

    // Testing crossover 
    std::cout << "Testing crossover method (x3)." << std::endl;
    id = test_crossover(20);
    cross_validate_(id, "Crossover");
    id = test_crossover(20);
    cross_validate_(id, "Crossover");
    id = test_crossover(100);
    cross_validate_(id, "Crossover");

    // Testing mutation 
    std::cout << "Testing mutation method (x3)." << std::endl;
    id = test_mutation(2, 0.8, 1);
    cross_validate_(id, "Mutation");
    id = test_mutation(20, 0.5, .01);
    cross_validate_(id, "Mutation");
    id = test_mutation(50, 1.0, .5);
    cross_validate_(id, "Mutation");

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
