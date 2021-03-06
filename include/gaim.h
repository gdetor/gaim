/* Header file "genetic_alg.h" of GAIM package 
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
 * @file gaim.h
 * Header file of GA and IM classes (and their methods), comprising the main
 * external interface.
 */
// $Log$
#ifndef GENETIC_ALG_H
#define GENETIC_ALG_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <libconfig.h++>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <sys/stat.h>

#include "pcg_random.hpp"

#define REAL_ float

#include <functional>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//#define TIME

#define XORSWAP(a, b)                                                         \
  ((&(a) == &(b)) ? (a)                                                       \
                  : ((a) ^= (b), (b) ^= (a),                                  \
                     (a) ^= (b))) /* checks that the addresses of a and b are \
                                     different before XOR-ing */


#define cross_validate_(id, name) {if(id) { \
                    printf("%s ", name); \
                    printf(ANSI_COLOR_RED "TEST FAILED\n" ANSI_COLOR_RESET); \
                    exit(-1);} \
                   else { \
                       printf("%s ", name); \
                       printf(ANSI_COLOR_GREEN "TEST PASSED\n" ANSI_COLOR_RESET );\
                   }}


/**
 * @brief Structure containing initialization parameters for the selection
 * operator. 
 *
 * A structure that contains all the parameters for specifying a selection
 * method and its corresponding parameters. It includes the selection method,
 * the number of parents (individuals) that will be selected for reproduction,
 * an integer k that defines the tournament size (how many individuals from 
 * the population will be drawn randomly), a replace parameter that determines
 * if the individuals will be selected with replacement or not. Furthermore, 
 * it contains the lower bound (lowest index) from which the selection begins
 * for the truncation method. Finally, includes the bias that controls how 
 * strongly the high rank individuals are favored when the Whithley selection
 * method is used.
 */
typedef struct parameter_sel {
    std::string selection_method; /**< Selection method. Can be one of: k-tournament,
                                    linear_rank, truncation, random, roulette,
                                    stochastic_roulette, and whitley*/
    REAL_ bias; /**<Whitley's method bias term (favors the high rank individuals */
    size_t num_parents; /**<The number of individuals from which to choose the
                          individulas that will reproduce */
    size_t lower_bound; /**< The starting index for the truncation selection method*/
    int k; /**< The tournament size for the k-tournament selection method */
    bool replace; /**< Defines if the selected individuals will be replaced or not */
} sel_parameter_s;


/**
 * @brief Structure containing initialization parameters for the crossover
 * operator.
 *
 * A structure that contains the parameters for specifying a crossover method.
 */
typedef struct parameter_cross {
    std::string crossover_method; /**< Crossover method. Can be one of: one_point,
    two_point, uniform, flat, discrete, and first_order. */
} cross_parameter_s;


/**
 * @brief Structure containing initialization parameters for mutation operator.
 *
 * A structure that contains all the parameters for specifying a mutation
 * operator. This data structure includes the mutation method, the mutation 
 * rate and variance (delta mutation), the lower and upper bounds for the fusion
 * and non-uniform mutation operators. Furthermore, it contains the time and 
 * order parameters for the nonuniform mutation operator and a parameter that
 * defines if the genome is of type int or real (float or double). 
 *
 */
typedef struct parameter_mut {
    std::string mutation_method; /**< Mutation method. It can be one: delta,
                                   random, nonuniform, fusion, and swap */
    REAL_ mutation_rate; /**< Mutation rate or probability threshold for delta
                           mutation operator */
    REAL_ variance;  /**< Mutation step for delta mutation operator */
    REAL_ low_bound; /**< Lower bound of interval [a, b] for all uniform 
                       distributions (only mutation operator) */
    REAL_ up_bound; /**< Upper bound of interval [a, b] for all uniform 
                      distributions (only mutation operator) */
    size_t order;  /**< Power of mutation rate (non-uniform mutation) */
    bool is_real;  /**< Determines if the genome is of type int or real
                     (float or double) */
} mut_parameter_s;


/**
 * @brief Structure containing initialization parameters for GA-based optimization. 
 *
 * A structure that contains all the parameters for specifying a Genetic Algorithms 
 * simulation, including how many generations it will run for, what the size of
 * the population is, what the size of the genome is, what the endpoints of the
 * interval to optimize on are, how many offspring are to be produced and how many
 * individuals are to be replaced.
 * It is typical in a Genetic Algorithm without elitism to replace the 
 * entire population. The number of indepedent runs can also be specified, if you are 
 * trying to derive an Monte Carlo estimate of fitness.
 */
typedef struct parameter_ga {
    sel_parameter_s sel_pms;    /**< Selection operator data structure */
    cross_parameter_s cross_pms;  /**< Crossover operator data structure */
    mut_parameter_s mut_pms;    /**< Mutation operator data structure */

    std::vector<REAL_> a;    /**< Lower bound of genome's interval ([a, b]) */
    std::vector<REAL_> b;    /**< Upper bound of genome's interval ([a, b]) */
    std::size_t generations;    /**< Number of generations the GA will run for */
    std::size_t population_size;    /**< Population size (number of individuals) */
    std::size_t genome_size;    /**< Size of genome of each individual */
    std::size_t num_offsprings; /**< Number of offsprings (children) */
    std::size_t num_replacement;    /**< Number of individuals being replaced in every generation */
    std::string clipping;    /**< Controls when the clipping values are the same for all genes
                               for every individual ("universal"), or each individual 
                               has its own clipping values per gene ("individual"),
                               or each individual has its own clipping values per gene
                               provided by a file ("file"). */
    std::string clipping_fname;     /**< Clipping values file name. Contains the clipping values 
                                      for each gene for each individual.*/
    int runs;   /**< Number of experimental runs, greater than 1 in case of multiple trajectories */
} ga_parameter_s;


/**
 * @brief Structure containing initialization parameters for an Island Model.
 *
 * A structure that contains all the parameters for running an Island Model
 * (IM), a GA with multiple subpopulations and structured migrations.
 */
typedef struct parameter_im {
    std::size_t num_immigrants; /**< Number of immigrants (individuals to move) */
    std::size_t num_islands;    /**< Number of islands (threads) in the model */
    std::size_t migration_interval; /**< Migration interval (period) */
    std::string pick_method;    /**< Name of method for selecting migrants */
    std::string replace_method; /**< Name of method for displacing residents */
    std::string adj_list_fname; /**< Name of the file containing adjacency list (pop. graph) */
    bool is_im_enabled = false; /**< Boolean flag indicating if IM is enabled or not */
} im_parameter_s;


/**
 * @brief Structure containing display parameters controlling which information
 * is printed.
 *
 * A structure that contains all the parameters for printing information about
 * the GA or the IM. 
 */
typedef struct parameter_pr {
    std::string where2write;    /**< Path where results files should be written. */
    std::string experiment_name;    /**< Name of the experiment */
    bool print_fitness;     /**< Boolean flag for printing individual fitnesses */
    bool print_average_fitness; /**< Boolean flag for printing average fitness of a population */
    bool print_bsf; /**< Boolean flag for printing best-so-far fitness of a population */
    bool print_best_genome; /**< Boolean flag for printing the best genome of a population */
} pr_parameter_s;


/**
 * @brief Structure representing an individual.
 *
 * A structure that represents an individual and carries all the necessary 
 * information about an individual, including ID, fitness, and genome.
 */
typedef struct individual {
    bool is_selected;   /**< Individual's selection flag (indicates if an 
                          individual has been selected for procreating */
    std::size_t id;     /**< Individual's Unique ID */
    REAL_ fitness;      /**< Individual's Fitness value */
    std::vector<REAL_> genome;  /**< Individual's Genome (vector of REAL_)*/
    std::vector<REAL_> lower_limit; /**< Individual's Genome lower boundary */
    std::vector<REAL_> upper_limit; /**< Individual's Genome upper boundary */
} individual_s;


/**
 * @brief Structure that holds the returned results from the ga_optimization
 * function. 
 *
 * A structure that keeps all the results (best genome, BSF, and average 
 * fitness). This data structure is used only from the ga_optimization 
 * function. 
 */
typedef struct ga_results {
    std::vector<REAL_> genome; /**< Best genome */
    std::vector<REAL_> bsf;   /**< BSF record */
    std::vector<REAL_> average_fitness;  /**< Average fitness record */
} ga_results_s;


#ifdef __cplusplus

/**
 * @brief Genetic Algorithm main class. 
 *
 * GA is the main class for seting up and running an optimization process. It
 * contains all the methods for performing operations such as selection, 
 * crossover, and mutation.
 */
class GA {
    public:
        GA(ga_parameter_s *); /**< Constructor method of GA class */
        ~GA();              /**< Destructor method of GA */

        /**
         * Genetic Algorithm basic operators
         */
        /// Selection operator methods
        std::vector<individual_s> ktournament_selection(std::vector<individual_s> &);
        std::vector<individual_s> truncation_selection(std::vector<individual_s> &);
        std::vector<individual_s> linear_rank_selection(std::vector<individual_s> &);
        std::vector<individual_s> random_selection(std::vector<individual_s> &);
        std::vector<individual_s> roulette_wheel_selection(std::vector<individual_s> &);
        std::vector<individual_s> stochastic_roulette_wheel_selection(std::vector<individual_s> &);
        std::vector<individual_s> whitley_selection(std::vector<individual_s> &);
        
        /// Selection pointer function
        std::vector<individual_s> (GA::*selection)(std::vector<individual_s> &);
        
        /// This method assigns the appropriate selection method to selection
        /// pointer function
        void select_selection_method(void);

        /// Crossover operator methods
        std::vector<REAL_> one_point_crossover(std::vector<REAL_>, std::vector<REAL_>);
        std::vector<REAL_> two_point_crossover(std::vector<REAL_>, std::vector<REAL_>);
        std::vector<REAL_> uniform_crossover(std::vector<REAL_>, std::vector<REAL_>);
        std::vector<REAL_> flat_crossover(std::vector<REAL_>, std::vector<REAL_>);
        std::vector<REAL_> discrete_crossover(std::vector<REAL_>, std::vector<REAL_>);
        std::vector<REAL_> order_one_crossover(std::vector<REAL_>, std::vector<REAL_>);

        /// Crossover pointer function
        std::vector<REAL_> (GA::*crossover)(std::vector<REAL_>, std::vector<REAL_>);
    
        /// This method assigns the appropriate crossover method to crossover
        /// pointer function
        void select_crossover_method(void);

        /// Mutation operator methods
        std::vector<REAL_> delta_mutation(std::vector<REAL_>);
        std::vector<REAL_> random_mutation(std::vector<REAL_>);
        std::vector<REAL_> nonuniform_mutation(std::vector<REAL_>);
        std::vector<REAL_> fusion_mutation(std::vector<REAL_>);
        std::vector<REAL_> swap_mutation(std::vector<REAL_>);
        
        /// Mutation pointer function
        std::vector<REAL_> (GA::*mutation)(std::vector<REAL_>);
    
        /// This method assigns the appropriate mutation operator based on
        /// the chosen mutation method
        void select_mutation_method(void);

        /**
         * Genetic Algorithm secondary operations
         */
        /// Evaluation of fitness of individuals
        void evaluation(std::vector<individual_s>&);
        /// Generate the next generation out of current individuals and
        // their offsprings
        void next_generation(size_t);
        /// Reset the flag is_selected (used from selections methods when
        // non-replacement is enabled
        void reset_selection_flags();
        /// Clips the values of genes for each individual based on its lower 
        // and upper boundaries (this works only when independent limits for
        // each gene are being used
        void clip_genome();
        /// Sorting individuals based on their fitness value
        void sort_population(void);
        /// Main routine for running one single generation step
        void run_one_generation(void);
        /// Main routine for evolving a population over generations
        void evolve(size_t, size_t, pr_parameter_s *); 

        std::vector<REAL_> &get_bsf(){ return bsf; }
        std::vector<REAL_> &get_best_genome(){ return bsf_genome; }
        std::vector<REAL_> &get_average_fitness(){ return fit_avg; }
        REAL_ (*fitness)(REAL_ *, size_t);

        std::vector<individual_s> population; /// Individuals population vector
        std::vector<individual_s> offsprings;   /// Offsprings vector 
        std::vector<individual_s> sorted_population;  /// Sorted population vector
        individual_s best_individual;   /// Best individual (keep track)
        std::vector<REAL_> bsf;     /// BSF vector (keep track)
        std::vector<REAL_> fit_avg;  /// Average fitness vector (keep track)
        std::vector<REAL_> hfi; /// Highest fitness in the population
        std::vector<REAL_> lfi; /// Lowest fitness in the population
        std::vector<REAL_> bsf_genome; /// BSF genome

    private:
        std::vector<REAL_> alpha, beta;  /// Genome's interval limits [a, b]
        std::vector<individual_s> immigrant;    /// Immigrants vector (buffer)
        std::string selection_method;
        std::string crossover_method;
        std::string mutation_method;
        REAL_ bias;    /// Bias term for Whitley selection metehod 
        REAL_ mutation_rate;
        REAL_ variance;
        REAL_ low_bound;
        REAL_ up_bound;
        size_t mu;     /// Number of individuals within a population
        size_t lambda; /// Number of offsprings
        size_t replace_perc;   /// Number of individuals being replaced 
        size_t genome_size;    /// Genome size (number of genes)
        size_t num_parents;    /// Number of parents per generation to select from
        size_t lower_bound;    /// Starting index for truncation selection
        size_t current_generation;
        size_t generations;
        size_t order;
        int k;                 /// k-tournament order
        bool replace;          /// If replace is enabled, then selected parents are replaced 
        bool is_real;

    friend class IM;
};


/**
 * @brief Island Model main class.
 * 
 * Island Model class contains methods to run an Island Model (IM)
 * optimization. The class instance reads the topology from a file and builds 
 * a map (adjacency list). Moreover, provides methods for individuals selection
 * and immigration over islands (threads). 
 */
class IM {
    public:
        IM(im_parameter_s *,
           ga_parameter_s *);
        ~IM();  /// IM Destructor method
        
        /**
         * Island Model basic operators methods
         */
        /// Selection method. It selects individuals per island to move and
        //populates the vacant spots with new ones randomly. 
        //@see select_ind2immigrate()
        void select_ind2migrate(size_t, size_t, std::string);
        /// Immigration method. It moves the immigrants (individuals) from one
        //island to another based on a specific policy.
        //@see move_immigrants()
        void move_immigrants(size_t, size_t, std::string);

        /**
         * Island Model secondary operations methods
         */
        /// Reads the connectivity graph (topology) from a file
        size_t read_connectivity_graph(std::string);
        /// Evolves an island (thread function)
        void evolve_island(size_t, im_parameter_s *, pr_parameter_s *);
        /// Runs the Island Models 
        void evolve_islands(im_parameter_s *, pr_parameter_s *);

        std::vector<GA> island; /// Islands (threads) vector

    private:
        std::vector<REAL_> a, b; /// Genome's interval [a, b]
        std::map<int, std::vector<int>> adj_list;   /// Adjacent list of islands
        size_t generations;     /// Generations
        size_t num_immigrants;  /// Number of immigrants
        size_t num_islands;     /// Number of islands (threads)
        size_t migration_interval;  /// Migration interval
        size_t migration_steps;    /// Generations / Migration Interval 

        std::mutex mtx;     // Mutex for locking threads
};


// Main island function
ga_results_s run_islands(REAL_ (*func)(REAL_ *, size_t),
                         im_parameter_s,
                         ga_parameter_s,
                         pr_parameter_s,
                         std::string);

// Auxiliary functions (only for C++)
void remove_at(std::vector<size_t>&, typename std::vector<size_t>::size_type);
size_t int_random(size_t, size_t);
REAL_ float_random(REAL_, REAL_);

#endif  /* __cplusplus  */

#ifdef __cplusplus
extern "C" {
#endif

/// Auxiliary Functions declarations
bool compare_fitness(const individual_s, const individual_s);
REAL_ average_fitness(REAL_, const individual_s);
bool is_path_exist(const std::string &);
int mkdir_(const std::string &);
int make_dir(const std::string &);
int remove_file(const std::string);
REAL_ calculate_whitley_factor(REAL_);
REAL_ maximum_fitness(std::vector<individual_s>);
REAL_ nonselected_maximum_fitness(std::vector<individual_s>);
REAL_ cumulative_fitness(std::vector<individual_s>);
REAL_ vector_norm(std::vector<REAL_>);
int argmin(std::vector<REAL_>);
int argmax(std::vector<REAL_>);
std::vector<REAL_> compute_population_norms(std::vector<GA>);
ga_results return_best_results(std::vector<GA>, std::string);



/// Multithread and Islands functions declarations
ga_results_s independent_runs(REAL_ (*func)(REAL_ *, size_t),
                              ga_parameter_s *,
                              pr_parameter_s *,
                              std::string);


/// Printing Functions
std::string center(const std::string, const int);
std::string prd(const double, const int, const int);
void print_fitness(std::vector<individual_s> &,
                   size_t unique_id=0,
                   std::string write_to="stdout");
void print_bsf(std::vector<REAL_>,
               size_t unique_id=0,
               std::string write_to="stdout");
void print_avg_fitness(std::vector<REAL_>,
                       size_t unique_id=0,
                       std::string write_to="stdout");
void print_best_genome(std::vector<REAL_>,
                       size_t unique_id=0,
                       std::string write_to="stdout");
void print_results(std::vector<individual_s>,
                   std::vector<REAL_>,
                   std::vector<REAL_>,
                   std::vector<REAL_>,
                   size_t, pr_parameter_s *);


/// Demo objective/fitness functions
/// End-user can define more at her/his will
// REAL_ sphere(std::vector<REAL_>&);
REAL_ sphere(REAL_ *, size_t);
REAL_ rastrigin(std::vector<REAL_>&);
REAL_ schwefel(std::vector<REAL_>&);
REAL_ griewank(std::vector<REAL_>&);
REAL_ tsm(std::vector<REAL_>& x);


/// Parameters files
std::tuple<ga_parameter_s,
           pr_parameter_s,
           im_parameter_s> read_parameters_file(const char *);
void print_parameters(ga_parameter_s,
                      pr_parameter_s,
                      im_parameter_s); 


/// Interface functions (to use GAIM through other programming languages
ga_results_s ga_optimization(REAL_ (*func)(REAL_ *, size_t),
                             size_t n_generations,
                             size_t population_size,
                             size_t genome_size,
                             size_t n_offsprings,
                             size_t n_replacements,
                             size_t n_rounds,
                             std::vector<REAL_> a,
                             std::vector<REAL_> b,
                             std::string clipping="universal",
                             std::string clipping_fname="clip_file.dat",
                             std::string selection_method="ktournament",
                             REAL_ bias=1.5,
                             size_t num_parents=2,
                             size_t lower_bound=1,
                             size_t k=2,
                             bool replace=false,
                             std::string crossover_method="one_point",
                             std::string mutation_method="delta",
                             REAL_ mutation_rate=0.5,
                             REAL_ mutation_var=0.5,
                             REAL_ low_bound=0.0,
                             REAL_ up_bound=1.0,
                             size_t order=1,
                             bool is_real=true,
                             bool is_im_enabled=false,
                             size_t n_islands=5,
                             size_t n_immigrants=4,
                             size_t migration_interval=500,
                             std::string pickup_method="elite",
                             std::string replace_method="poor",
                             std::string im_graph_fname="star_graph.dat",
                             std::string experiment_id="experiment-1",
                             std::string log_path="./data/",
                             std::string return_type="minimum",
                             bool log_fitness=false,
                             bool log_average_fitness=true,
                             bool log_bsf=true,
                             bool log_best_genome=true);


void ga_optimization_python(REAL_ (*func)(REAL_ *, size_t),
                            size_t n_generations,
                            size_t population_size,
                            size_t genome_size,
                            size_t n_offsprings,
                            size_t n_replacements,
                            size_t n_rounds,
                            REAL_ *a,
                            REAL_ *b,
                            char *clipping,
                            char *clipping_fname,
                            char *selection_method,
                            REAL_ bias,
                            size_t num_parents,
                            size_t lower_bound,
                            size_t k,
                            bool replace,
                            char *crossover_method,
                            char *mutation_method,
                            REAL_ mutation_rate,
                            REAL_ mutation_var,
                            REAL_ low_bound,
                            REAL_ up_bound,
                            size_t order,
                            bool is_real,
                            bool is_im_enabled,
                            size_t n_islands,
                            size_t n_immigrants,
                            size_t migration_interval,
                            char *pickup_method,
                            char *replace_method,
                            char *im_graph_fname,
                            char *experiment_id,
                            char *log_path,
                            char *return_type,
                            bool log_fitness,
                            bool log_average_fitness,
                            bool log_bsf,
                            bool log_best_genome,
                            REAL_ **genoe,
                            REAL_ **bsf,
                            REAL_ **agv_fitness);

#ifdef __cplusplus
}
#endif
#endif  // GENETIC_ALG_H
