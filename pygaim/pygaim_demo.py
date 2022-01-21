# Test/Demo script for pyGAIM.
# Copyright (C) 2020  Georgios Detorakis (gdetor@protonmail.com)
#                     Andrew Burton (ajburton@uci.edu)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# $Id$
#
# @file pygaim_demo.py
# Implements a very basic fitness function (sphere) and a simple GA.
#
# $Log$
import numpy as np
import pygaim


def error(x):
    """!
    A simple test fitness function (Sphere).
    """
    x = np.array(x, 'd')
    tmp = (x**2).sum()
    return -tmp


if __name__ == '__main__':
    # Define all GA parameters
    sel_params = pygaim.sel_parameter_s()
    sel_params.selection_method = "random"
    sel_params.bias = 1.5
    sel_params.num_parents = 2
    sel_params.lower_bound = 1
    sel_params.k = 2
    sel_params.replace = False

    cross_params = pygaim.cross_parameter_s()
    cross_params.crossover_method = "one_point"

    mut_params = pygaim.mut_parameter_s()
    mut_params.mutation_method = "delta"
    mut_params.mutation_rate = 0.5
    mut_params.variance = 0.5
    mut_params.low_bound = 0.0
    mut_params.up_bound = 1.0
    mut_params.time = 1
    mut_params.order = 1
    mut_params.is_real = True

    ga_params = pygaim.ga_parameter_s()
    ga_params.sel_pms = sel_params
    ga_params.mut_pms = mut_params
    ga_params.cross_pms = cross_params
    ga_params.generations = 1000    # Number of generations
    ga_params.population_size = 200     # Number of individuals
    ga_params.genome_size = 2           # Number of parameters to search for
    ga_params.a = [-10.0, -10.0]     # Lower bound of fitness
    ga_params.b = [10.0, 10.0]      # Upper bound of fitness
    ga_params.num_offsprings = 5        # Number of offsprings
    ga_params.num_replacement = 3       # Number of individuals to be replaced
    ga_params.runs = 1                  # Number of indipendent runs of a GA
    ga_params.clipping = "universal"     # Global clipping values
    ga_params.clipping_fname = "../examples/clip_file.dat".encode('utf-8')

    # Define all Logging parameters
    where2write = "./data/".encode('utf-8')     # Where to store the results
    experiment_name = "PyGATest".encode('utf-8')    # Name of the experiment
    log_params = pygaim.pr_parameter_s()
    log_params.where2write = where2write
    log_params.experiment_name = experiment_name
    log_params.print_fitness = True
    log_params.print_average_fitness = True
    log_params.print_bsf = True
    log_params.print_best_genome = True

    # Define all IM parameters
    # im_params = IslandModelParams()
    # im_params.num_immigrants = 3
    # im_params.num_islands = 5
    # im_params.migration_interval = 100
    # im_params.pick_method = "elit".encode('utf-8')
    # im_params.replace_method = "poor".encode('utf-8')
    # im_params.adj_list_fname = "./examples/star_graph.dat".encode('utf-8')
    # im_params.is_im_enabled = False

    new_ga = pygaim.GA(ga_params)   # GA instance
    new_ga.pycallback(error)        # Assign the fitness function
    new_ga.evolve(1000, 0, log_params)  # Evolve the GA#0 for 1000 generations

    print(new_ga.get_best_genome())
