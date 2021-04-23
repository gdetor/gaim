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
    ga_parameter_s ga_test;
    ga_test.a = -1;
    ga_test.b = 1;
    ga_test.generations = 1000;
    ga_test.population_size = 30;
    ga_test.genome_size = 2;
    ga_test.num_offsprings = 5;
    ga_test.num_replacement = 3;
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


parameter_im init_im_params(void)
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


int test_evolve_islands(parameter_im *im_pms)
{
    ga_parameter_s ga_pms(init_ga_params());
    pr_parameter_s pr_pms(init_print_params()); 
    IM im(im_pms, &ga_pms);

    im.run_islands(im_pms, &pr_pms);
    return 0;
}


int test_im(std::size_t num_immigrants, std::size_t migration_interval,
            std::string method)
{
    int id;
    parameter_im pms(init_im_params());
    pms.num_immigrants = num_immigrants;
    pms.pick_method = method;
    pms.replace_method = method;
    pms.migration_interval = migration_interval;

    pms.adj_list_fname = "./examples/all2all_graph.dat";
    std::cout <<  "All-2-all topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method << 
        " migration policy";
    id = test_evolve_islands(&pms);
    cross_validate_(id, "");

    pms.num_islands = 4;
    pms.adj_list_fname = "./examples/ring_graph.dat";
    std::cout <<  "Ring topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method <<
        " migration policy";
    id = test_evolve_islands(&pms);
    cross_validate_(id, "");

    pms.num_islands = 5;
    pms.adj_list_fname = "./examples/star_graph.dat";
    std::cout <<  "Star topology " << num_immigrants << " immigrants "
        << migration_interval << " migration interval and " << method <<
        " migration policy";
    id = test_evolve_islands(&pms);
    cross_validate_(id, "");
    return 0;
}


int main()
{
    std::cout << "Test Island Model" << std::endl;
    test_im(3, 500, "random");
    test_im(3, 500, "elit");
    test_im(3, 500, "poor");
    test_im(2, 100, "elit");
    test_im(4, 100, "elit");
    test_im(5, 100, "poor");
    return 0;
}
