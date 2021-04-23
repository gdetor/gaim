# pyGAIM tests script
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
# @file pygaim_tests.py
# Implements tests for the Python wrappers of pyGAIM.
#
# $Log$

import pygaim as pg
import numpy as np

# np.random.seed(13)


if __name__ == '__main__':
    a = np.random.random(5)
    b = np.random.random(5)

    # Test crossover methods
    print("#"*40)
    print("Test Crossover Methods")
    print("Original parents genomes")
    print("Parent #1: ", a)
    print("Parent #2: ", b)
    print("#"*40)
    res = pg.one_point_crossover(a, b)
    print("1PT CX: ", res)
    res = pg.two_point_crossover(a, b)
    print("2PT CX: ", res)
    res = pg.uniform_crossover(a, b)
    print("Uniform CX: ", res)
    res = pg.flat_crossover(a, b)
    print("Flat CX: ", res)
    res = pg.discrete_crossover(a, b)
    print("Discrete CX: ", res)
    res = pg.order_one_crossover(a, b)
    print("Order-One CX: ", res)
    print()

    # Test selection methods
    print("#"*40)
    print("Test Selection Methods")
    print("Original Population")
    population = [pg.individual_s() for _ in range(5)]
    for i in range(5):
        population[i].id = i+1
        population[i].is_selected = 0
        population[i].fitness = np.random.randint(0, 12)
        population[i].genome = np.random.randint(0, 6, (3,))
        print(population[i].genome, population[i].fitness)
    print("#"*40)

    res = pg.ktournament_selection(population, 3, 2, True)
    for i in range(len(res)):
        print("Ktournament Selected: ", res[i].genome)

    res = pg.truncation_selection(population, 2, 2, True)
    for i in range(len(res)):
        print("Truncation Selected: ", res[i].genome)

    res = pg.linear_rank_selection(population, 2, True)
    for i in range(len(res)):
        print("Linear RANK Selected: ", res[i].genome)

    res = pg.random_selection(population, 2, True)
    for i in range(len(res)):
        print("RANDOMS Selected: ", res[i].genome)

    res = pg.roulette_wheel_selection(population, 2, True)
    for i in range(len(res)):
        print("RW Selected: ", res[i].genome)

    res = pg.stochastic_roulette_wheel_selection(population, 2, True)
    for i in range(len(res)):
        print("SRW Selected: ", res[i].genome)

    res = pg.whitley_selection(population, 1.5, 2, True)
    for i in range(len(res)):
        print("Whitley Selected: ", res[i].genome)
    print()
    print()

    # Test mutation methods
    genome = np.array([1, 2, 3, 2])
    genome = genome.astype('f')
    print("#"*40)
    print("Test Mutation Methods")
    print("Original Genome: ", genome)
    print("#"*40)

    res = pg.delta_mutation(genome, 0.5, 1.0)
    print("DELTA MUT: ", res)
    res = pg.random_mutation(genome, 0, 9, True)
    print("RANDOM MUT: ", res)
    res = pg.nonuniform_mutation(genome, 5, 1000, 1, 5, 14)
    print("NONUNIFORM MUT: ", res)
    res = pg.fusion_mutation(genome, 5, 10, True)
    print("FUSION MUT: ", res)
    res = pg.swap_mutation(genome)
    print("SWAP MUT: ", res)
