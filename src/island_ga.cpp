/* Island Model Class cpp file for GAIM software 
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
 * @file island_ga.cpp
 * Defines the Island Model class, which runs a genetic algorithm 
 * process across multiple communicating subpopulations.
 */
// $Log$
#include "gaim.h"
#include <utility>
// #include "barrier.h"


/* std::mutex mtx;     // Mutex for locking threads */
// _pthread_barrier_t barrier; // Barrier for sync threads */
pthread_barrier_t barrier; // Barrier for sync threads */


/**
 * @brief Constructor of Island Model (IM) class
 *
 * Initializes all the necessary parameters and vectors for running an
 * Island Model and evolve different islands (threads) based on a 
 * predermined Genetic Algorithm (GA). 
 *
 * @param[in] im_pms Structure of IM parameters
 * @param[in] ga_pms Structure of GA parameters
 * @return Nothing
 */
IM::IM(im_parameter_s *im_pms, ga_parameter_s *ga_pms)
{
    size_t num_vertices;
    a = ga_pms->a;  // Lower bound of genome [a, b]
    b = ga_pms->b;  // Upper bound of genome [a, b]
    num_islands = im_pms->num_islands;      // Number of islands
    num_immigrants = im_pms->num_immigrants;    // Number of immigrants
    migration_interval = im_pms->migration_interval;    // Migration interval

    if (migration_interval > ga_pms->generations) {
        std::cerr << "Migration interval exceeds number of generations!" 
            << std::endl;
        exit(-1);
    }

    // Compute the migration steps 
    migration_steps = ga_pms->generations;

    // Read the connectivity graph from a file
    num_vertices = read_connectivity_graph(im_pms->adj_list_fname);
    if (num_vertices != num_islands) {
        std::cerr << "WARNING: Number of islands does not match the number of vertices!"
            << std::endl;
        std::cout << "Number of islands has changed to: " << num_vertices 
            << std::endl;
        num_islands = num_vertices;     // Assign number of islands
        im_pms->num_islands = num_vertices;
    }

    // Initialize the GA for each island
    for (size_t i = 0; i < num_islands; ++i) {
       island.push_back(GA(ga_pms));
    }
}


/**
 * @brief Destructor of IM class
 */
IM::~IM()
{
    island.clear();
    adj_list.clear();
}


/**
 *  Reads the connectivity graph (topology) from a text file. The graph defines
 *  the number of islands (run on separate threads) and how they are connected. 
 *  The format of the file is as follows:
 *  XX (number of islands - vertices)
 *  DST \#edges SRC1 SRC2 SRC3
 *
 *  Example (ring topology with four islands):
 *  4
 *  0 1 3
 *  1 1 0
 *  2 1 1
 *  3 1 2
 *
 *  @param[in] fname The name of the file that contains the graph
 *  @return num_vertices The number of vertices (islands) found in the file.
 *
 *  @note The number of vertices is returned in order to validate the number of
 *  islands. In the rest of the code if the number if islands provided by the
 *  user does not match the number of vertices then the latter is being used as
 *  number of islands (threads). 
 */
size_t IM::read_connectivity_graph(std::string fname)
{
    int source, content;
    size_t num_vertices, num_edges;

    auto ifile = std::fstream(fname, std::ios::in);
    if (!ifile) {
        std::cout << "Unable to open file " << fname << std::endl;
        exit(1);
    }

    ifile >> num_vertices;
    for (size_t i = 0; i < num_vertices; ++i) {
        ifile >> source;
        ifile >> num_edges;
        for (size_t j = 0; j < num_edges; ++j) {
            ifile >> content;
            if (content < 0 && content > (int) num_islands) {
                std::cerr << "Error: Invalid destination island!" << std::endl;
                exit(-1);
            }
            adj_list[source].push_back(content);
        }
    }
    ifile.close();
    return num_vertices;
}


/**
 * Selects the candidate individuals for immigration. The current method
 * implements three distinct selection methods.
 * @li random   The number of individuals is selected randomly from the
 *              population
 * @li elit     Individuals with the best fitness (elit) are selected
 * @li poor     Individuals with the worst fitness (poor) are selected
 *
 * @param num_immigrants Number of total individuals who are selected to
 *                       migrate
 * @param unique_id      Is the unique ID of each island (thread ID)
 * @param method         Defines whith strategy (policy) of selecting
 * individuals is going to be used (elit, poor, random)
 * @return Nothing (void)
 *
 * @note In a future version of GAIM, each island will accept a different 
 * number of immigrants.
 */
void IM::select_ind2migrate(size_t num_immigrants,
                            size_t unique_id,
                            std::string method)
{
    size_t id;
    size_t len = island[unique_id].population.size();
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> probs(a[0], b[0]);
    std::vector <REAL_> new_genome(island[unique_id].genome_size);
    std::vector<int> pop(island[unique_id].population.size());

    std::iota(std::begin(pop), std::end(pop), 0);
    std::shuffle(std::begin(pop), std::end(pop), gen);

    mtx.lock();
    island[unique_id].immigrant.clear();
    if (method == "random") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            id = pop[i];
            island[unique_id].immigrant.push_back(island[unique_id].sorted_population[id]);
        }
    } else if (method == "elit") {
        for (size_t i = len-1, j = 0; j < num_immigrants; --i, ++j) {
            island[unique_id].immigrant.push_back(island[unique_id].sorted_population[i]);
        }
    } else if (method == "poor") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            island[unique_id].immigrant.push_back(island[unique_id].sorted_population[i]);
        }
    } else {
        std::cerr << "No such immigration method exists!" <<std::endl;
        exit(-1);
    }
    mtx.unlock();

    for (size_t i = 0; i < num_immigrants; ++i) {
        std::generate(new_genome.begin(), new_genome.end(), [&]{return probs(gen);});
        island[unique_id].population[island[unique_id].immigrant[i].id].genome = new_genome;
        island[unique_id].population[island[unique_id].immigrant[i].id].fitness = island[unique_id].fitness(&new_genome[0], new_genome.size());
    }
}


/**
 * Moves immigrants from one island (thread) to another based on a predefined
 * method. Supported methods are:
 * @li random Individuals from the current island are randomly chosen and they
 *            are replaced by immigrants
 * @li elit   The highest-fitness individuals within the current island 
 *            population are replaced by immigrants
 * @li poor   The lowest-fitness individuals within the current island
 *            population are replaced by immigrants  
 *
 * @param num_immigrants Total number of immigrants the island can accept
 * @param unique_id Island's unique ID (thread ID)
 * @param method How individuals from the local population will be replaced by
 *               immigrants
 * @return Nonthing (void)
 *
 * @note In a future version of GAIM, islands will support a variable number
 * of immigrants.
 */
void IM::move_immigrants(size_t num_immigrants,
                         size_t unique_id,
                         std::string method="random")
{
    size_t id;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<individual_s> tmp(num_immigrants);
    std::vector<int> pop(island[unique_id].population.size());

    std::iota(std::begin(pop), std::end(pop), 0);
    std::shuffle(std::begin(pop), std::end(pop), gen);

    mtx.lock();
    for (auto &k : adj_list[unique_id]) {
        if (method == "random") {
            size_t i = 0;
            for (auto &t : island[k].immigrant) {
                id = pop[i];
                island[unique_id].population[id].genome = t.genome;
                island[unique_id].population[id].fitness = 
                    island[unique_id].fitness(&t.genome[0], t.genome.size());
                i++;
            }
        } else if (method == "poor") {
            size_t i = 0;
            for (auto &t : island[k].immigrant) {
                island[unique_id].sorted_population[i].genome = t.genome;
                island[unique_id].sorted_population[i].fitness = 
                    island[unique_id].fitness(&t.genome[0], t.genome.size());
                i++;
            }
            island[unique_id].population = island[unique_id].sorted_population;
        } else if (method == "elit") {
            id = island[unique_id].population.size();
            size_t i = 0;
            for (auto &t : island[k].immigrant) {
                island[unique_id].sorted_population[id-1-i].genome = t.genome;
                island[unique_id].sorted_population[id-1-i].fitness = 
                    island[unique_id].fitness(&t.genome[0], t.genome.size());
                i++;
            }
            island[unique_id].population = island[unique_id].sorted_population;
        } else {
            std::cerr << "No such immigration method exists!" <<std::endl;
            exit(-1);
        }
    }
    mtx.unlock();
}


/**
 * Evolves an island. This method runs the genetic algorithm per island and 
 * performs two major operations, selection of outgoing individuals (migrants)
 * and replacement of the target population. Furthermore,
 * it provides logging for the results of the evolution process per island.
 *
 * @param unique_id Unique ID number of island (thread ID)
 * @param im_pms Island model parameters structure
 * @param pr_pms Logging parameters structure
 * @return Nothing (void)
 */
void IM::evolve_island(size_t unique_id,
                       im_parameter_s *im_pms,
                       pr_parameter_s *pr_pms)
{
    std::vector<individual_s> immigrant;

    island[unique_id].evaluation(island[unique_id].population);
    for (size_t k = 0; k < migration_steps; ++k) {
        island[unique_id].run_one_generation();
        pthread_barrier_wait(&barrier);

        if (!(k % migration_interval)) {
            select_ind2migrate(im_pms->num_immigrants,
                               unique_id,
                               im_pms->pick_method);
            pthread_barrier_wait(&barrier);
            move_immigrants(im_pms->num_islands,
                            unique_id,
                            im_pms->replace_method);
            pthread_barrier_wait(&barrier);
        }
    }
    island[unique_id].sort_population();
    pthread_barrier_wait(&barrier);
    
    mtx.lock();
    if (pr_pms->print_fitness) {
        print_fitness(island[unique_id].population,
                      unique_id,
                      pr_pms->where2write);
    }
    if (pr_pms->print_average_fitness) {
        print_avg_fitness(island[unique_id].fit_avg,
                          unique_id,
                          pr_pms->where2write);
    }
    if (pr_pms->print_bsf) {
        print_bsf(island[unique_id].bsf, unique_id, pr_pms->where2write);
    } 
    if (pr_pms->print_best_genome) {
        print_best_genome(island[unique_id].best_individual.genome,
                          unique_id,
                          pr_pms->where2write);
    }
    mtx.unlock();
}


/**
 * Runs an Island Model experiment. 
 *
 * @param im_pms Island model parameters structure
 * @param pr_pms Logging parameters structure
 * @return Nothing (void)
 */
void IM::run_islands(im_parameter_s *im_pms, pr_parameter_s *pr_pms)
{
    pthread_barrier_init(&barrier, NULL, im_pms->num_islands);
    std::vector<std::thread> islands;

    for (size_t i = 0; i < num_islands; ++i) {
        islands.push_back(std::thread(&IM::evolve_island,
                                      this,
                                      i,
                                      im_pms,
                                      pr_pms));
    }
    for (std::thread& th : islands) {
        if (th.joinable()) { th.join(); }
    }
}
