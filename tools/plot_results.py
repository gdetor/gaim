# GAIM plotting tools (Python)
# Copyright (C) 2019  Georgios Detorakis (gdetor@protonmail.com)
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
# @file plot_results
# Implements basic plotting functions for GAs results illustrations
#
# $Log$

import os
import sys
import glob
import numpy as np
from struct import unpack
import matplotlib.pylab as plt
import matplotlib.ticker as mticker


def plot_data(directory='./data/',
              data_type="bsf",
              nbytes=4,
              average=False,
              ax=None):
    """
    Plots either the Best-so-far (BSF) fitness or the average fitness per
    population.

    Args:
        directory (string): The full path to the directory the data are stored
        data_type (string): bsf-for best-so-far fitness or average_fitness for
                            the average fitness per population
        nbytes (int):       Number of bytes of the encoding. Default is 4 (int
                            or float). Use 8 for double or long int.
        average (bool):     If enabled (True) averages the fitness records over
                            all individuals.
        ax (obj):           A subplot object in case one would like to use
                            subplots or grids.

    Returns:
    """
    if os.path.isdir(directory) is not True:
        print("Directory does not exist!")
        sys.exit(-1)

    fnames = glob.glob(directory+'/*.dat')

    i = 0
    data = []
    for name in fnames:
        if data_type in name:
            with open(name, 'rb') as f:
                c = f.read()
            if nbytes == 4:
                tmp = np.array(unpack('f'*(len(c)//4), c), 'f')
            elif nbytes == 8:
                tmp = np.array(unpack('d'*(len(c)//8), c), 'd')
            else:
                print("Number of bytes is not valid!")
                sys.exit(-1)
            data.append(tmp)
    data = np.array(data)
    if average:
        average_fit = data.mean(axis=0)

    if ax is None:
        fig = plt.figure(figsize=(9, 5))
        fig.subplots_adjust(bottom=0.2, left=0.2)
        ax = fig.add_subplot(111)
    if average:
        ax.plot(average_fit, lw=2)
    else:
        for i in range(len(data)):
            ax.plot(data[i], lw=2)
            plt.ticklabel_format(axis="y", style='sci')
    # ax.set_ylim([data.min(), 0])
    ax.set_xlabel("Generations", fontsize=16, weight='bold')
    if data_type == "bsf":
        ax.set_ylabel("BSF", fontsize=16, weight='bold')
    else:
        ax.set_ylabel("Average fitness", fontsize=16, weight='bold')
    ticks = ax.get_xticks().astype('i')
    ax.xaxis.set_major_locator(mticker.FixedLocator(ticks))
    ax.set_xticklabels(ticks, fontsize=16, weight='bold')
    ticks = np.round(ax.get_yticks(), 6)
    ax.yaxis.set_major_locator(mticker.FixedLocator(ticks))
    ax.set_yticklabels(ticks, fontsize=16, weight='bold')


def print_fitness(directory='./data/', islands=3, pop_size=[20, 20, 20],
                  nbytes=4):
    """
    Prints the fitness per island/population.

    Args:
        directory (string): The full path to the directory the data are stored
        islands (int):      Number of Islands in the model (or independent
                            runs)
        pop_size (int list): Population size for each island
        nbytes (int):       Number of bytes of the encoding. Default is 4 (int
                            or float). Use 8 for double or long int.

    Returns:
    """
    for i in range(islands):
        fname = directory+"fitness_"+str(i)+".dat"
        f = open(fname, 'rb')
        print("Island # %d" % (i))
        for _ in range(pop_size[i]):
            c, d = f.read(4), f.read(nbytes)
            idx = unpack('i', c)
            if nbytes == 4:
                fit = unpack('f', d)
            elif nbytes == 8:
                fit = unpack('d', d)
            else:
                print("Number of bytes is not valid!")
                sys.exit(-1)

            print(idx[0], fit[0])
        print("---------------------------------------")


def print_best_genome(directory='./data/', islands=1, nbytes=4):
    """
    Prints the best genome per island/population.

    Args:
        directory (string): The full path to the directory the data are stored
        islands (int):      Number of Islands in the model (or independent
                            runs)
        nbytes (int):       Number of bytes of the encoding. Default is 4 (int
                            or float). Use 8 for double or long int.

    Returns:
    """
    data = []
    for i in range(islands):
        fname = directory+"best_genome_"+str(i)+".dat"
        with open(fname, 'rb') as f:
            c = f.read()
        if nbytes == 4:
            size = len(c) // 4
            dec = unpack('f'*size, c)
        elif nbytes == 8:
            size = len(c) // 8
            dec = unpack('d'*size, c)
        else:
            print("Number of bytes is not valid!")
            sys.exit(-1)
        data.append(dec)

    for i, d in enumerate(data):
        print("Best Genome of Island # %d" % (i))
        print(d)


if __name__ == '__main__':
    path = sys.argv[1]
    # with plt.xkcd():
    fig = plt.figure(figsize=(16, 6))

    ax = fig.add_subplot(121)
    # Plot BSF
    plot_data(directory=path, data_type="bsf", nbytes=4, ax=ax)
    # ax.set_ylim([-2, 0])
    ax.text(0, 0.05, 'A',
            ha='left',
            fontsize=21,
            weight='bold')

    # Plot average fitness
    ax = fig.add_subplot(122)
    plot_data(directory=path, data_type="average_fitness", nbytes=4, ax=ax)
    # ax.set_ylim([-1.5, 0])
    ax.text(0, 0.07, 'B',
            ha='left',
            fontsize=21,
            weight='bold')
    # plt.savefig("bsf_avg_fit.png")

    # Pring individuals IDs and their Fitness
    # print_fitness(directory=path)

    # Print the best Genome
    # print_best_genome(directory=path, nbytes=8)
    plt.show()
