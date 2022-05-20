/*
 * Tests for the logging system of GAIM
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
#include <unistd.h>


ga_parameter_s init_ga_params(void)
{
    std::vector<REAL_> a(2, -1.0);
    std::vector<REAL_> b(2, 1.0);
    mut_parameter_s mut;
    cross_parameter_s cross;
    sel_parameter_s sel;
    ga_parameter_s ga_test;

    sel.selection_method = "ktournament";
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
    pr_test.where2write = "./test_data/";
    pr_test.experiment_name = "test_";
    pr_test.print_fitness = true;
    pr_test.print_average_fitness = true;
    pr_test.print_bsf = true;
    pr_test.print_best_genome = true;
    return pr_test;
}


int test_prints(void) {
    int id;
    int count = 0;
    std::string fitness("fitness_0.dat"), bsf("bsf_0.dat");
    std::string avg_fitness("average_fitness_0.dat");
    std::string genome("best_genome_0.dat"), base("./test_data/");
    ga_parameter_s ga_pms(init_ga_params());
    pr_parameter_s pr_pms(init_print_params());

    mkdir_(base);

    GA gen_alg(&ga_pms);
    gen_alg.evolve(1000, 0, &pr_pms);

    // Check if files have been created
    id = 1 - is_path_exist(base+fitness);
    cross_validate_(id, "Print fitness");
    if (!id) {
        count++;
        remove_file(base+fitness);
    }

    id = 1 - is_path_exist(base+bsf);
    cross_validate_(id, "Print BSF");
    if (!id) {
        count++;
        remove_file(base+bsf);
    }

    id = 1 - is_path_exist(base+avg_fitness);
    cross_validate_(id, "Print Average Fitness");
    if (!id) {
        count++;
        remove_file(base+avg_fitness);
    }

    id = 1 - is_path_exist(base+genome);
    cross_validate_(id, "Print Best Genome");
    if (!id) {
        count++;
        remove_file(base+genome);
    }

    rmdir(base.c_str());

    if (count == 4) {
        std::cout << "Test passed: " << count << " / 4" << std::endl;
        return 0;
    } 
    else {
        std::cout << "Test failed: " << (4 - count) << " / 4" << std::endl;
        return 1;
    }
}


int main() 
{
    test_prints();
    return 0;
}
