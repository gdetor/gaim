import numpy as np
import ctypes as C
import matplotlib.pylab as plt


def error(x: float, length: int):
    """!
    A simple test fitness function (Sphere).
    """
    address = C.addressof(x.contents)
    x = np.ctypeslib.as_array((C.c_float * length).from_address(address))
    tmp = (x**2).sum()
    return -float(tmp)


class GAOptimize():
    def __init__(self,
                 objective_func,
                 n_generations=1000,
                 population_size=50,
                 genome_size=3,
                 n_offsprings=10,
                 n_replacements=5,
                 n_rounds=1,
                 a=[-1, -1, -1],
                 b=[1, 1, 1],
                 clipping='universal',
                 clipping_fname="clip_values_file.dat",
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
                 n_immigrants=4,
                 migration_interval=500,
                 pickup_method="elit",
                 replace_method="poor",
                 im_graph_fname="./examples/star_graph.dat",
                 experiment_id="experiment-1",
                 log_fname="./data/",
                 log_fitness=False,
                 log_average_fitness=True,
                 log_bsf=True,
                 log_best_genome=True):
        self.n_generations = n_generations
        self.population_size = population_size
        self.genome_size = genome_size
        self.n_offsprings = n_offsprings
        self.n_replacements = n_replacements
        self.n_rounds = n_rounds
        self.clipping = self.string2pchar(clipping)
        self.clipping_fname = self.string2pchar(clipping_fname)
        self.selection_method = self.string2pchar(selection_method)
        self.bias = bias
        self.num_parents = num_parents
        self.lower_bound = lower_bound
        self.k = k
        self.replace = replace
        self.crossover_method = self.string2pchar(crossover_method)
        self.mutation_method = self.string2pchar(mutation_method)
        self.mutation_rate = mutation_rate
        self.mutation_var = mutation_var
        self.low_bound = low_bound
        self.up_bound = up_bound
        self.order = order
        self.is_real = is_real
        self.is_im_enabled = is_im_enabled
        self.n_islands = n_islands
        self.n_immigrants = n_immigrants
        self.migration_interval = migration_interval
        self.pickup_method = self.string2pchar(pickup_method)
        self.replace_method = self.string2pchar(replace_method)
        self.im_graph_fname = self.string2pchar(im_graph_fname)
        self.experiment_id = self.string2pchar(experiment_id)
        self.log_fname = self.string2pchar(log_fname)
        self.log_fitness = log_fitness
        self.log_average_fitness = log_average_fitness
        self.log_bsf = log_bsf
        self.log_best_genome = log_best_genome

        arr_dtype = C.POINTER(C.c_float*self.genome_size)
        self.a = np.array(a, 'f').ctypes.data_as(arr_dtype)
        self.b = np.array(b, 'f').ctypes.data_as(arr_dtype)

        self.genome = np.zeros((self.genome_size,), 'f')
        self.genome_p = self.genome.ctypes.data_as(C.POINTER(C.c_float))

        self.bsf = np.zeros((self.n_generations,), 'f')
        self.bsf_p = self.bsf.ctypes.data_as(C.POINTER(C.c_float))

        self.avg = np.zeros((self.n_generations,), 'f')
        self.avg_p = self.avg.ctypes.data_as(C.POINTER(C.c_float))

        library_path = "/home/gdetorak/packages/gaim/lib/libgaim.so"
        pygaim = C.cdll.LoadLibrary(library_path)
        self.ga_optimize = pygaim.ga_optimization_python

        self.CMPFUNC = C.CFUNCTYPE(C.c_float,
                                   C.POINTER(C.c_float*self.genome_size),
                                   C.c_size_t)

        self.ga_optimize.argtypes = [self.CMPFUNC,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.POINTER(C.c_float*self.genome_size),
                                     C.POINTER(C.c_float*self.genome_size),
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_float,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_bool,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_float,
                                     C.c_float,
                                     C.c_float,
                                     C.c_float,
                                     C.c_size_t,
                                     C.c_bool,
                                     C.c_bool,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_size_t,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_char_p,
                                     C.c_bool,
                                     C.c_bool,
                                     C.c_bool,
                                     C.c_bool,
                                     C.POINTER(C.POINTER(C.c_float)),
                                     C.POINTER(C.POINTER(C.c_float)),
                                     C.POINTER(C.POINTER(C.c_float))]
        self.ga_optimize.restype = None

        self.callback = self.CMPFUNC(objective_func)

    def fit(self):
        self.ga_optimize(self.callback,
                         self.n_generations,
                         self.population_size,
                         self.genome_size,
                         self.n_offsprings,
                         self.n_replacements,
                         self.n_rounds,
                         self.a,
                         self.b,
                         self.clipping,
                         self.clipping_fname,
                         self.selection_method,
                         self.bias,
                         self.num_parents,
                         self.lower_bound,
                         self.k,
                         self.replace,
                         self.crossover_method,
                         self.mutation_method,
                         self.mutation_rate,
                         self.mutation_var,
                         self.low_bound,
                         self.up_bound,
                         self.order,
                         self.is_real,
                         self.is_im_enabled,
                         self.n_islands,
                         self.n_immigrants,
                         self.migration_interval,
                         self.pickup_method,
                         self.replace_method,
                         self.im_graph_fname,
                         self.experiment_id,
                         self.log_fname,
                         self.log_fitness,
                         self.log_average_fitness,
                         self.log_bsf,
                         self.log_best_genome,
                         C.byref(self.genome_p),
                         C.byref(self.bsf_p),
                         C.byref(self.avg_p))
        return self.genome, self.avg, self.bsf

    def string2pchar(self, instring):
        # return instring.encode('ascii')
        return bytes(instring, 'utf-8')

    def plot_(self):
        def pretty_ticks(ax):
            ticks = ax.get_xticks().astype('i')
            ax.set_xticklabels(ticks, fontsize=16, weight='bold')
            ticks = np.round(ax.get_yticks(), 4)
            ax.set_yticklabels(ticks, fontsize=16, weight='bold')

        fig = plt.figure(figsize=(14, 5))
        fig.subplots_adjust(wspace=0.3, hspace=0.3)
        ax = fig.add_subplot(121)
        ax.plot(ga.bsf, 'k', lw=2)
        ax.set_xlabel("Generations", fontsize=16, weight='bold')
        ax.set_ylabel("BSF", fontsize=16, weight='bold')
        pretty_ticks(ax)
        ax = fig.add_subplot(122)
        ax.plot(ga.avg, 'orange', lw=2)
        ax.set_xlabel("Generations", fontsize=16, weight='bold')
        ax.set_ylabel("Average Fitness", fontsize=16, weight='bold')
        pretty_ticks(ax)


if __name__ == '__main__':
    genome_size = 2
    ga = GAOptimize(error,
                    n_generations=500,
                    population_size=20,
                    genome_size=genome_size,
                    n_offsprings=5,
                    n_replacements=2,
                    a=[float(-1) for _ in range(genome_size)],
                    b=[float(1) for _ in range(genome_size)])
    ga.fit()
    ga.plot_()
    plt.show()
