/* GAIM Interface function -- C++ example file 
 * Copyright (C) 2019  Georgios Detorakis (gdetor@protonmail.com)
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
 * @file main_interface.cpp
 * How to use the main interface of gaim to simplify your life.
 */
// $Log$
#include "gaim.h"


int main() {
    py_results res;
    std::string replace = "poor", choice = "elite";
    std::string store = "./data/", exp_id = "interface";
    std::string clip = "universal", clip_file = " ", graph = " ";

    int n = 2;
    std::vector<REAL_> a(n, -1.0), b(n, 1.0);
    res = ga_optimization(sphere,       // fitness function
                          1000,         // Generations
                          10,           // Population size
                          2,            // Genome size
                          3,            // Number of offsprings
                          1,            // Replacements
                          1,            // Independent rounds
                          a,            // a - lower bound
                          b);            // b - upper bound

    std::cout << "Best genome:" << std::endl;
    for (auto &g: res.genome) {
        std::cout << g << "  ";
    }
    std::cout << std::endl;
    return 0;
}
