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
    int res;
    char *replace = "poor", *choice = "elite";
    char *store = "./data/", *exp_id = "interface";
    char *clip = " ", *graph = " ";

    res = ga_optimization(sphere,       // fitness function
                          1000,         // Generations
                          10,           // Population size
                          2,            // Genome size
                          3,            // Number of offsprings
                          1,            // Replacements
                          1,            // Independent rounds
                          5,            // Number of Islands
                          4,            // Number of immigrants
                          200,          // Migration interval
                          -10.0,        // a - lower bound
                          10.0,         // b - upper bound
                          choice,       // Pickup method (IM)
                          replace,      // Replace method (IM)
                          clip,         // Clipping filename
                          store,        // Where to store the logs
                          exp_id,       // Experiment ID
                          graph,        // IM connectivity graph file
                          false,        // Log fitness
                          true,         // Log average fitness
                          true,         // Log BSF
                          true,         // Log of best genome
                          true,         // Enable/disable genome universal clipping 
                          false);       // Enable/disable the IM
    return 0;
}
