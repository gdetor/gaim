# GAIM

GAIM is a flexible and lightweight C++ library implementing Genetic Algorithms (GA) and the Island
Models (IM) that incorporate them for real-type genomes. For more information about GAs, IMs, and evolutionary computation (EC) in general, see (Eiben and Smith 2003, Engelbrecht 2007, de Jong 2006, or Mitchell 1998). 

The library can be called from C/C++ (since an ABI is already implemented) and supports multithreading
for running the sort of independent experiments needed to establish Monte Carlo average and best-so-far estimates of attained fitness.
 
In addition, GAIM can be integrated with other packages such as neural simulators
([CarlSim](https://github.com/UCI-CARL/CARLsim4)) or deep learning frameworks ([C++ Pytorch API](https://pytorch.org/cppdocs/)) in order to perform neuroevolution (i.e. to evolve the weights and other model parameters of artificial neural networks).

> GAIM was not designed with binary GAs in mind (the operators do not constrain the genome to bitstrings, and the representations are floating point numbers). Further, GAIM does not currently
provide support for specifying constraints to the optimization.

> GAIM reads a libconfig-style text file to specify evolutionary parameters. An example configuration file can be found in `examples/` directory under the name `demo.cfg`.

> Future releases will support divide-and-conquer Island Models.

## Dependencies 
Building GAIM requires the following packages: 
- GCC (6.0.0>)
- libconfig++ (9v5> and dev) 
- pthreads 
- OpenMPI
- Pybind11 (only if pyGAIM is used)


## Install
There are two options for installing GAIM's dependencies: manual or semi-automatic. We
recommend using the manual installation -- required packages tend to change
over time and across Linux distros, and your package manager may advise you that
you already have some of the dependencies installed on your system.

Once all the dependencies are satisfied, then GAIM can be used in your programs 
(the More Examples section lists provided example programs that include GAIM 
code, and briefly details how to build a redistributable shared library).

### Manual Installation of Dependencies

First, the user has to make sure that GCC 6.0 (or greater) is installed, 
with `g++` and `make`. You should also verify that the pthreads library can be 
used -- a partial implementation is packaged with libc in many cases, but some 
platforms furnish special libpthread extensions.

The following commands exemplify installing libpthread's extensions for Ubuntu 
and for Fedora distro families:
```
sudo apt install make gcc g++ libpthread-stubs0-dev (Ubuntu/Debian)
sudo dnf install make gcc g++ libpthread-stubs (Fedora/Redhat)
sudo pacman -S base-devel gcc make (Arch)
```
Then libconfig should be installed. This can be done in the following way:
```
sudo apt install libconfig++9v5 libconfig++-dev (Ubuntu/Debian)
sudo dnf install libconfig++9v5 libconfig++-dev (Fedora/Redhat)
sudo pacman -S libconfig (Arch)
```
Finally, if you would like to use the MPI capabilities of GAIM, you have
to install OpenMPI:
```
sudo apt install openmpi-common openmpi-bin openmpi-doc libopenmpi2 libopenmpi-dev (Ubuntu/Debian)
sudo dnf install openmpi (Fedora/RedHat)
sudo pacman -S openmpi (Arch)
```

### Semi-automatic installation of dependencies
The second option is to run the script `install.sh`.

The script installs all the dependencies and then compiles the project putting the
executables under the `bin/` directory. In case you choose to compile as a library
the script will put the `.so` file in the `lib` directory.

If you use the installation script, you need to provide sudo access to it.

The script takes three arguments. The first defines how many threads you would
like to use to accelerate the compilation process (n >= 1). The second argument
enables the compilation of the tests (pass 1 to enable it) and the last argument
enables the compilation of the GAIM as a dynamic library (pass 1 to enable). 
In any case, the script will try to install all the necessary dependencies based
on your Linux distro. 

To compile with tests and on one thread, yielding a shared library:
```
sudo ./install.sh 1 1 1
```


## Main classes and methods (see documentation for more details)

-`GA`: Main class implementing a Genetic Algorithm. This class contains all
the necessary methods for a standard GA such as selection, crossover, mutation, evaluation,
sorting, and refinement of fitness over generations. A selection of alternative operators are pre-defined separately.

-`IM`: Main class implementing an Island Model. This class contains all the
methods that are necessary for running an Island Model. It provides methods
for selecting individuals from islands and moving them towards other islands
within the model.

-`independent_runs`: Main function for running independent GAs using threads
and logging information about the outcomes of the separate experiments.

-`interface`: Or **ga_optimization()** is a function that provides one-line
optimizization (similar to Scipy's minimize). The user passes the objective
function as an argument along with all the necessary optimization parameters.
In this case there is no need for a configuration file. The same function can
be called from Python since GAIM already provides a Python wrapper based on 
Pybind11. 


## Controlling the optimizer using GAIM configuration files

All the parameters for running an experiment have to be given by providing a configuration file
(of the format read by libconfig).  Here we explain what is in a typical GAIM configuration file.

The first block is just the name of the experiment. Every experiment you
run can (and should) have a unique name so that you can easily keep track of your results.

``` 
name = "GA_demo_simulation"; 
```

The second block of parameters controls the GA-based optimization itself. This block
is the most important and can never be omitted.

```
GA =
{
    evolution = {
        runs = 1;                   // Number of independent runs
        generations = 1000;         // Total number of generations
        population_size = 20;       // Number of individuals within a population
        number_of_offsprings = 10;  // Number of offspring (children)
        size_of_genome = 2;         // Number of genes (genome size)    
        number_of_replacement = 3;  // How many individuals will be replaced by offspring
        universal_clipping = true;  // Enables universal clipping for gene values within an 
                                    // interval (a, b). If it's false then a file should provide
                                    // indipendent values for each gene/individual
        clipping_fname = "./examples/clip_file.dat"     // File with all the clipping values
                                    // for each gene/individual
        a = -1.0;                   // Lower bound of genome values (for initialization)
        b = 1.0;                    // Upper bound of genome values (for initialization)
    }
```
GAIM accepts two types of boundary conditions (limits) of genome. The first (when 
universal clipping is set to TRUE) all the genes within a population are bouded
from the same numbers within an interval \[a, b\] (the same values we use for the
initialization of the genome are being used to clip the genome). If the universal
clipping is set to FALSE then each gene of each individual has its own upper 
and lower limits. These limits are given to GAIM via a text file (see 
clipping\_fname parameter in the code snippet above). The text file has to 
be organized as follows: The first line contains the lower limits of the first
individual's genome. The second line provides the upper limits. The third line
contains the lower limit of the second individual and so on so forth. 


The third block of parameters allow you to control logging. These parameters
indicate what kind of information is going to be displayed to STDOUT or written
to a file. 

```
    print = {
        where_to_write = "./data/";     // Either "stdout" or a path to the directory
                                        // where files will be stored
        print_fitness = false;          // Enables the logging of individual fitnesses
        print_average_fitness = true;   // Enables the logging of the average fitness of a population
        print_bsf = true;               // Enables the logging of best-so-far (BSF) fitness
        print_best_genome = false;      // Enables the logging of the best genome within a population
        }
```

The last block provides parameters for the Island Model, which runs the GA process
across multiple communicating subpopulations. Island-based processing can be disabled if
you want an ordinary GA. 

```
    island_model = {
        im_enabled = false;         // Enables the infrastructure or Island Model algorithm
        island_model_graph_file = "./examples/ring_graph.dat"   // Path to the graph (connectivity) file
        number_of_immigrants = 3;   // Number of individuals that will migrate
        number_of_islands = 3;      // Number of Islands (threads)
        migration_interval = 500;   // Migration interval (how often an immigration takes place)
        candidate_immigrants_pickup_method = "random";  // How we choose individuals to move
        candidate_immigrants_replace_method = "elit";   // How we replace individuals when newcomers arrive
        }
};

```

Whereas the space of options for most of the previous config parameters are largely
implicit (file paths or numbers controlling the configuration of the GA), the 
`candidate_immigrants_pickup_method` and `candidate_immigrants_replace_method`
parameters refer to specific hardcoded options available in the IM class.

Current options are `"random"`, `"elit"`, or `"poor"` corresponding to whether 
random individuals, the lowest-fitness individuals, or the highest-fitness 
individuals are chosen for migration and removal, respectively. 

More information is available within the documentation, which also details 
built-in mutation, selection, and crossover operators. GAIM's design towards 
flexibility (allowing plugging in replacement functions at multiple levels of 
the optimizer) is intended in light of the fact that there are not generally
problem-agnostic sets of highly-recommended evolutionary hyperparameters 
(de Jong 2006 introduces some facets of this issue for toy problems and across 
non-GA forms of evolutionary optimization, such as evolutionary programming and 
evolutionary strategies; Engelbrecht 2007 references argumentation related to 
the No Free Lunch theorem).

## Example usage

Using GAIM once the dependencies are installed primarily involves writing custom 
configuration files and using a small amount of sample code in your program. 

In the section Optimizing A Function Of Your Choice, the typical GAIM skeleton code
is shown (read the config file with `read_parameters_file` and use a `GA` object
with a custom fitness function and call the evolve function for a specified number
of steps). This effectively constitutes a end-to-end example of incorporating GAIM,
minus a full discussion of all of the prepackaged operators detailed in the documentation.

Immediately below, though, we focus first on manipulating the configuration files and 
acquainting you with the location of the variant mode example files (independent runs,
MPI, and island model) provided with the project.

### Minimizing a Schwefel function with an ordinary GA
First we introduce GAIM with a very simple example. We will minimize the 
Schwefel function `$418.9829 \times D - \sum_{i=1}^{D} x_i \sin()\sqrt{|x_i|}$`.
This is a standard benchmark function for optimizers and has a global minimum at `$f({\bf x}^*) = 0$`
where `${\bf x}^* = (420.9687, \ldots, 420.9687)$`. `$D$` is the dimension of 
the space (which, in this case, will be 10). This means that the genome of each individual 
will be a 10-dimensional vector. We choose 20 individuals and we will evolve the population
for 5000 generations. We choose to generate 10 offspring at each generation and we 
replace only 5 of the parents. The interval in which genomes are constrained is 
[-500, 500]. 
When all of these parameters are passed to the configuration file, it will look like this:
```
name = "schwefel_experiment";

GA =
{
    evolution = {
        runs = 1;                   // Number of independent runs
        generations = 5000;         // Total number of generations
        population_size = 20;       // Number of individuals within a population
        number_of_offsprings = 10;  // Number of offspring (children)
        size_of_genome = 10;         // Number of genes (genome size)    
        number_of_replacement = 5;  // How many individuals will be replaced by offspring
        a = -500.0;                   // Lower bound of genome (for initialization)
        b = 500.0;                    // Upper bound of genome (for initialization)
    }
```
Since we only desire a simple GA, not an island model which generalizes the GA, we ignore the last block of the configuration
file. We store the configuration file as *schwefel.cfg* and then we compile the 
project and execute the code: 
```
$ make binary
(or make -j 5 binary)
$ ./bin/main 
```
Once this program terminates, we can examine the results, which will be stored in the directory 
**data**. All of the relevant files start with the name *schwefel_experiment*, as we specified at the top of the configuration file. 


### Running an Island Model

Now, we will minimize the Schwefel function using an Island Model. In this case, we 
keep the same GA block in the configuration file as in the previous section
and we change the name and add the IM block. The most important parameter is
the *im_enabled* which enables the island model computation. Arbitrarily, we choose to have 3 islands and 
move 5 individuals from and to each island every 500 generations. The way we 
choose the individuals to move is set to random and we replace the BEST individuals
of the destination islands with the immigrant individuals. The final configuration
block expressing these wishes looks like:

```
    name = "im_schwefel_experiment";

    island_model = {
        im_enabled = true;         // Enables the infrastructure or Island Model algorithm
        island_model_graph_file = "./examples/ring_graph.dat"   // Path to the graph (connectivity) file
        number_of_immigrants = 5;   // Number of individuals that will migrate
        number_of_islands = 3;      // Number of Islands (threads)
        migration_interval = 500;   // Migration interval (how often an immigration takes place)
        candidate_immigrants_pickup_method = "random";  // How we choose individuals to move
        candidate_immigrants_replace_method = "elit";   // How we replace individuals when newcomers arrive
        }
```
We run the program just as we did in the previous paragraph (compiling it first if we have not).

### More examples

> More examples can be found under the directory **examples**! The user can find
more detailed ways to use all of the functionality provided by GAIM. In 
the **examples** directory one can find the following files:
```
examples/
├── all2all_graph.dat   (all-to-all connectivity graph)
├── demo.cfg    (demonstration configuration file)
├── demo_independent_runs.cfg (independent runs configuration file)
├── demo_island_model.cfg (Island Model configuration file)
├── main_custom_ga.cpp  (How to customize the GA algorithm)
├── main_independent_run.cpp    (independent runs main cpp file)
├── main_island_model.cpp   (island model main cpp file)
├── main_mpi_island_model.cpp (island model MPI main cpp file)
├── main_lib.cpp    (How to call GAIM as dynamic library *.so)
├── main_mpi_independent_runs.cpp   (MPI independent runs example)
├── Makefile
├── ring_graph.dat  (ring topology file)
├── star_graph.dat  (star topology file)
└── tsp.cfg
```

The user can run the following command to compile each of the aforementioned
source files: 
```
$ cd examples/
$ make custom_ga            # compiles the custom GA file
$ make independent_runs     # compiles GA indepenent runs 
$ make island_model         # compiles an Island Model example
$ make mpi_independent_runs # compiles MPI GA independent runs
$ make mpi_island           # compiles MPI Island Model
```

Regarding the MPI implementations, the user can run them using the following
commmand
```
$ mpirun -n X mpi_island
$ mpirun -n X mpi_independent_runs
```
Where `X` is the number of MPI processes and it **must** be the same with the
number of islands in the model (see the configuration file `demo_island_model.cfg`).
If the user needs to run on more processes (by default this is the number of 
cores per CPU) can use the hostfile in the **examples** directory and run:

```
$ mpirun --hostfile hostfile -np X mpi_island
$ mpirun --hostfile hostfile -np X mpi_independent_runs
```
> :warning: **MPI implementation**: If you'd like to use the MPI example files
you have to pay attention to the way the adjacency matrix of the connectivity
graph is defined. Please refer to the documentation. Furthermore, if you'd like
to run the MPI IM example, you have to change the path of the connectivity 
file from *ring_graph.dat* to *mpi_ring_graph.dat* in **demo_island_model.cfg**.


For using GAIM as a dynamic library (.so), one has to compile the project as a
dynamic library first running the following command under the GAIM directory.
```
$ pwd
/path/to/GAIM
$ mkdir lib
$ make library
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/lib
$ cd examples/
$ make lib_exe
$ ./lib_exe
```
Results from using GAIM in this manner are still stored by default in the **data/** directory.

### Optimizing a function of your choice

In this example we demonstrate how the user can define their own fitness 
function for optimization. To begin with, we choose a test function that is 
not implemented in the GAIM package. This function will be
`$f(x, y) = 100 \sqrt{|y - 0.01 x^2|} + 0.01 |x + 10|$`. The function is known as 
the No. 6 Bukin function and has zero as the global minimum at `$(-10, 1)$`. In order 
to minimize this function, we will use the default GA. First we set all the 
parameters as we did in the previous examples. This means that we set all 
the parameters in a file named **bukin.cfg**. So we create the file, and we 
add the following snippet. 

```
name = "bukin6_experiment";

GA =
{
    evolution = {
        runs = 1;                   // Number of indepdent runs
        generations = 3000;         // Total number of generations
        population_size = 20;       // Number of individuals within a population
        number_of_offsprings = 10;  // Number of offsprings (children)
        size_of_genome = 2;         // Number of genes (genome size)    
        number_of_replacement = 5;  // How many individuals will be replaced by offsprings
        a = -10.0;                   // Lower bound of genome
        b = 10.0;                    // Upper bound of genome
    }

    island_model = {
        im_enabled = false;         // Enables the infrastructure or Island Model algorithm
        island_model_graph_file = "./examples/ring_graph.dat"   // Path to the graph (connectivity) file
        number_of_immigrants = 5;   // Number of individuals that will migrate
        number_of_islands = 3;      // Number of Islands (threads)
        migration_interval = 500;   // Migration interval (how often an immigration takes place)
        candidate_immigrants_pickup_method = "random";  // How we choose individuals to move
        candidate_immigrants_replace_method = "elit";   // How we replace individuals when new comers arrive
        }
```
The next step is to define the function in the source code. This can be done 
by modifying the *main.cpp* in the directory **gaim/**. The main file should 
look like:
```
#include "gaim.h"


REAL_ bukin6(std::vector<REAL_>& x)
{
    return 100 * sqrt(fabs(x[1] - 0.01 * x[0]*x[0]|) + 0.01 * fabs(x[0] + 10);
}


int main() {
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("bukin6.cfg");
    print_parameters(ga_pms, pr_pms, im_pms);

    GA gen_alg(&ga_pms);
    gen_alg.fitness = &bukin6;
    gen_alg.evolve(ga_pms.generations, 0, &pr_pms);
    return 0;
}
```
Essentially, we define the function (bukin6), then we instantiate the GA class, assigning bukin6 to serve as the fitness function, and invoke the optimizer. 

Once we have both the configuration file (specifying the optimization parameters) and the code example which makes use of GAIM in place, we compile the code and execute it. We can then inspect the results
stored in the directory *./data/*.
```
$ make binary
$ ./bin/main
```
> The user can specify where the **bukin6.cfg** file lives. In this example we
assumed that it's in the same directory as the *main.cpp* file. 

Once the user has the configuration file and the main function they desire, they can link to the dynamic library instead of compiling the entire GAIM package (see the section
**More examples** above for instructions on creating the dynamic library).


## Optimize a function through Python with pyGAIM

GAIM offers Python wrappers based on [pybind11](https://github.com/pybind/pybind11).
So far the user can call GAIM via Python and can define their own fitness function
written in Python. In the future we will provide wrappers for the entire GAIM 
infrastracture so the user will be able to define their own GA flow. 

The user has to compile the source code using the Makefile in the **pygaim**
directory. 
```
$ cd pygaim/
$ make PYTHON=1
```

If everything runs smoothly and the code compiles without any error then the
user can import the pyGAIM in their source code:
```
import pygaim
```

Then, they can define a fitness function 
```
def error(x):
    """!
    A simple test fitness function (Sphere).
    """
    x = np.array(x, 'd')
    tmp = (x**2).sum()
    return -tmp
```

And the GA and logging parameters (see [here](https://github.com/gdetor/gaim#controlling-the-optimizer-using-gaim-configuration-files)). 
```
if __name__ == '__main__':
    # Define all GA parameters
    ga_params = pygaim.ga_parameter_s()
    ga_params.a = -10.0     # Lower bound of fitness
    ga_params.b = 10.0      # Upper bound of fitness
    ga_params.generations = 1000    # Number of generations
    ga_params.population_size = 200     # Number of individuals
    ga_params.genome_size = 2           # Number of parameters to search for
    ga_params.num_offsprings = 5        # Number of offsprings
    ga_params.num_replacement = 3       # Number of individuals to be replaced
    ga_params.runs = 1                  # Number of indipendent runs of a GA

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
```
Finally, they can instantiate the GA class, assign the fitness function to the
callback method and call the evolve method to run the GA. 

```
    new_ga = pygaim.GA(ga_params)   # GA instance
    new_ga.pycallback(error)        # Assign the fitness function
    new_ga.evolve(1000, 0, log_params)  # Evolve the GA#0 for 1000 generations
```
The complete example can be found in the **pygaim** directory as a standalone
python script under the name **pygaim_demo.py**.


## Optimize a function through the interface function from python
The following code snippet demonstrates how we can use the **ga_optimization**
function to minimize any objective function. For the moment the objective
function accepts only one argument, an array that represents the genome. It
does not accept any extra parameters so if the user would like to have some 
extra parameters they will have to wrap their objective function around a new
function that gets as an argument only the genome. Future versions of GAIM
will allow an objective function with extra parameters. 


```
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
                                 1000,         # generations 
                                 20,           # population size
                                 2,            # genome size (parameters)
                                 5,            # number of offsprings
                                 3,            # number of replacements
                                 1,            # number of independent experiments
                                 [-1., -1.],   # lower bound of interval [a, b]
                                 [1., 1.],     # upper bound of interval [a, b]
                                 "universal",  # clipping type
                                 "clip_file",  # clipping values file name
                                 "ktournament", # selection method
                                 1.5,           # Whitley method's bias
                                 2,             # number of parents
                                 1,             # starting index for truncation
                                 2,             # k-tournament order
                                 false,         # replace switch
                                 "one_point",   # crossover method
                                 "delta",       # mutation method
                                 0.5,           # mutation rate
                                 0.1,           # mutation variance
                                 0.0,           # lower bound of RNG (mutation)
                                 1.0,           # upper bound of RNG (mutation)
                                 1,             # order of nonuniform mutation
                                 True,          # real/int switch (mutation)
                                 False,         # enable/disable IM
                                 5,             # number of islands
                                 3,             # number of immigrants
                                 500,           # migration interval
                                 "elite",       # immigrant pickup method (IM)
                                 "poor",        # individual replace method (IM)    
                                 "graph.dat",   # IM graph file
                                 "test",        # experiment ID
                                 "./data/",     # directory where to store
                                 True,          # store fitness record
                                 True,          # store average fitness
                                 True,          # store BSF record
                                 True)          # store best genome
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
    print(res.genome)       # print the best genome
```

The function **ga_optimization** returns a data structure of type
**py_results_s**. This data structure contains the best genome, the BSF (over
generations), and the average fitness (over generations).



## Platforms where GAIM has been tested

- Ubuntu 20.04.1 LTS
  - GCC 9.3.0 
  - Python 3.8.2
  - x86_64 64bit

- Arch Linux
  - GCC 8.2.1 20180831
  - Python 3.7.0
  - x86_64 64bit

- Ubuntu 18.04.2 LTS
  - GCC 7.3.0 
  - Python 2.7.15+
  - x86_64 64bit


## Contributing Guidelines

Please first consult the **CONTRIBUTING.md** file for the Code of Conduct.

In case you would like to contribute to GAIM, you should use the [Github Pull
Request](https://github.com/gdetor/gaim/pulls) system. 


## For more help

See documentation included under doc/. 

## Report Bugs

In case you would like to report a bug or you experience any problems with GAIM,
you should open an issue using the [Github Issue Tracker](https://github.com/gdetor/gaim/issues). 

## References

* De Jong, K. A. (2006). Evolutionary Computation. MIT Press.
* A.E. Eiben, and J.E. Smith, (2003). Introduction to evolutionary computing. Springer, 53.
* Engelbrecht, A. P. (2007). Computational Intelligence: An Introduction (2nd ed.). Wiley.
* Mitchell, M. (1998). An Introduction to Genetic Algorithms. MIT Press.

## Cite
Please use following citation to cite GAIM in scientific publications:

G. Detorakis and A. Burton, *GAIM: A C++ library for Genetic Algorithms and Island Models*, 
Journal of Open Source Software 4 (44), 1839.

### Bibtex format
  ```
  @article{detorakis2019gaim,
    title={GAIM: A C++ library for Genetic Algorithms and Island Models},
    author={Detorakis, Georgios and Burton, Andrew},
    journal={Journal of Open Source Software},
    volume={4},
    number={44},
    pages={1839},
    year={2019}
  }
  ```


 
