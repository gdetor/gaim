/* GAIM Library C++ example file for GAIM software 
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
 * @file main_lib.cpp
 * How to use the GAIM as library
 */
// $Log$
#include "gaim.h"
#include <iostream>

/*
 * This file demonstrates how the GAIM library can be called from a C++ 
 * code. 
 */
int main() {
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    // Read the parameters from a configuration file
    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("../examples/demo.cfg");
    // Print all the parameters either on stdout or to a file
    print_parameters(ga_pms, pr_pms, im_pms);

    // Instantiate GA class
    GA gen_alg(&ga_pms);
    // Run an evolution experiment
    gen_alg.evolve(ga_pms.generations, 0, &pr_pms);

    return 0;
}
