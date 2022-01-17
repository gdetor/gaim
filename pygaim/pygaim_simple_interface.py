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
                                 10,           # Population size
                                 2,            # Genome size
                                 3,            # Number of offsprings
                                 1,            # Replacements
                                 1,            # Independent rounds
                                 5,            # Number of Islands
                                 4,            # Number of immigrants
                                 200,          # Migration interval
                                 [-10.0, -10.0],   # a - lower bound
                                 [10.0, 10.0],     # b - upper bound
                                 "universal",   # Clipping method
                                 " ",          # Clipping filename
                                 "lolo",       # Experiment ID
                                 "./data/",     # Where to store the logs
                                 "elite",      # Pickup method (IM)
                                 "poor",       # Replace method (IM)
                                 " ",          # IM connectivity graph file
                                 False,        # Log fitness
                                 True,         # Log average fitness
                                 True,         # Log BSF
                                 True,         # Log of best genome
                                 False)        # Enable/disable the IM
