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
    ga_parameter_s ga_test;
    ga_test.a = -1;
    ga_test.b = 1;
    ga_test.generations = 1000;
    ga_test.population_size = 20;
    ga_test.genome_size = 2;
    ga_test.num_offsprings = 5;
    ga_test.num_replacement = 3;
    ga_test.runs = 1;
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
