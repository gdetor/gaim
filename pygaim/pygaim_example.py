# import numpy as np
import matplotlib.pylab as plt

from pygaim import GAOptimize, c2numpy


def error(x: float, length: int):
    """!
    A simple test fitness function (Sphere).
    """
    x = c2numpy(x, length)
    tmp = (x**2).sum()
    return -float(tmp)


if __name__ == '__main__':
    genome_size = 2
    ga = GAOptimize(error,
                    n_generations=500,
                    population_size=20,
                    genome_size=genome_size,
                    n_offsprings=5,
                    n_replacements=2,
                    n_rounds=1,
                    a=[float(-1) for _ in range(genome_size)],
                    b=[float(1) for _ in range(genome_size)])
    ga.fit()
    ga.plot_()
    plt.show()
