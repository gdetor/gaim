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
    res = pygaim.ga_optimization(error,        # fitness function
                                 1000,         # Generations
                                 20,           # Population size
                                 3,            # Genome size
                                 10,           # Number of offsprings
                                 5,            # Replacements
                                 1,            # Independent rounds
                                 [-1., -1., -1.],   # a - lower bound
                                 [1., 1., 1.],     # b - upper bound
                                 clipping="universal",
                                 clipping_fname="clamp_fname.dat",
                                 selection_method="ktournament",
                                 bias=1.5,
                                 num_parents=2,
                                 lower_bound=1,
                                 k=2,
                                 replace=False,
                                 crossover_method="one_point",
                                 mutation_method="delta",
                                 mutation_rate=0.5,
                                 mutation_var=0.5,
                                 low_bound=0.0,
                                 up_bound=1.0,
                                 order=1,
                                 is_real=True,
                                 is_im_enabled=False,
                                 n_islands=5,
                                 n_immigrants=3,
                                 migration_interval=500,
                                 pickup_method="elite",
                                 replace_method="poor",
                                 im_graph_fname="graph.dat",
                                 experiment_id="experiment1",
                                 log_fname="./data/",
                                 log_fitness=False,
                                 log_average_fitness=True,
                                 log_bsf=True,
                                 log_best_genome=True)
    print(res.genome)
