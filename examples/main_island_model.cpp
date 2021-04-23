/* GAIM Island Model Standalone C++ example file
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
 * @file main_island_model.cpp
 * How to run an Island Model using GAIM
 */
// $Log$
#include "gaim.h"

int main() {
    // Define all the tuples and structures
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    // Load the parameters from the demo_island_model configuration file
    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("demo_island_model.cfg");
    print_parameters(ga_pms, pr_pms, im_pms);

    // Instantiate the IM and run it
    std::cout << "Running an Island Model" << std::endl;
    IM island_model(&im_pms, &ga_pms);
    island_model.run_islands(&im_pms, &pr_pms);

    return 0;
}
