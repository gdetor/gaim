/*
 * Tests for Island Model
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
    pr_test.where2write = "stdout";
    pr_test.experiment_name = "test_";
    pr_test.print_fitness = false;
    pr_test.print_average_fitness = false;
    pr_test.print_bsf = false;
    pr_test.print_best_genome = false;
    return pr_test;
}


im_parameter_s init_im_params(void)
{
    im_parameter_s im_test;
    im_test.num_immigrants = 3;
    im_test.num_islands = 3;
    im_test.migration_interval = 500;
    im_test.pick_method = "random";
    im_test.replace_method = "random";
    im_test.is_im_enabled = true;
    im_test.adj_list_fname = "./tests/graph1.dat";
    return im_test;
}


int test_evolve_islands(im_parameter_s im_pms)
{
    ga_parameter_s ga_pms(init_ga_params());
    pr_parameter_s pr_pms(init_print_params()); 
    // IM im(im_pms, &ga_pms);
    std::string return_type("minimum");
    ga_results_s res;

    res = run_islands(sphere, im_pms, ga_pms, pr_pms, return_type);
    return 0;
}


int test_im(std::size_t num_immigrants, std::size_t migration_interval,
            std::string method)
{
    int id;
    im_parameter_s pms(init_im_params());
    pms.num_immigrants = num_immigrants;
    pms.pick_method = method;
    pms.replace_method = method;
    pms.migration_interval = migration_interval;

    pms.adj_list_fname = "./examples/all2all_graph.dat";
    std::cout <<  "All-2-all topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method << 
        " migration policy";
    id = test_evolve_islands(pms);
    cross_validate_(id, "");

    pms.num_islands = 4;
    pms.adj_list_fname = "./examples/ring_graph.dat";
    std::cout <<  "Ring topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method <<
        " migration policy";
    id = test_evolve_islands(pms);
    cross_validate_(id, "");

    pms.num_islands = 5;
    pms.adj_list_fname = "./examples/star_graph.dat";
    std::cout <<  "Star topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method <<
        " migration policy";
    id = test_evolve_islands(pms);
    cross_validate_(id, "");
    return 0;
}


int main()
{
    std::cout << "Test Island Model" << std::endl;
    test_im(3, 500, "random");
    test_im(3, 500, "elite");
    test_im(3, 500, "poor");
    test_im(2, 100, "elite");
    test_im(4, 100, "elite");
    test_im(5, 100, "poor");
    return 0;
}
