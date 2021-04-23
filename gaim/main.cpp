/*
 * Main cpp file for GAIM software 
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

int main() {
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("./examples/demo.cfg");
    print_parameters(ga_pms, pr_pms, im_pms);

    if (im_pms.is_im_enabled) {
        std::cout << "Running an Island Model" << std::endl;
        IM island_model(&im_pms, &ga_pms);
        for (size_t i = 0; i < im_pms.num_islands; ++i) {
           island_model.island[i].fitness=&sphere;
        }
        island_model.run_islands(&im_pms, &pr_pms);
    } else {
        if (ga_pms.runs == 1) {
            std::cout << "Running a single GA" << std::endl;
            GA gen_alg(&ga_pms);
            // gen_alg.fitness = &tsm;
            gen_alg.fitness = &sphere;
            gen_alg.evolve(ga_pms.generations, 0, &pr_pms);
        } else if (ga_pms.runs > 1) {
            std::cout << "Running "<< ga_pms.runs << " Independent GAs" << std::endl;
            independent_runs(&ga_pms, &pr_pms);
        } else {
            std::cout << "Negative number of runs is illegal!" << std::endl;
            exit(-1);
        }
    }
    return 0;
}
