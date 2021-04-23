/* GAIM Independent runs of a GA -- C++ example file 
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
 * @file main_independent_runs.cpp
 * How to run independent runs of a GA using POSIX threads.
 */
// $Log$
#include "gaim.h"


int main() {
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("demo_independent_runs.cfg");
    print_parameters(ga_pms, pr_pms, im_pms);

    std::cout << "Running "<< ga_pms.runs << " Independent GAs" << std::endl;
    independent_runs(&ga_pms, &pr_pms);
    return 0;
}
