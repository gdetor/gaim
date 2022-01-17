/* GAIM MPI Island Model -- C++ example file 
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
 * @file main_mpi_island_model.cpp
 * How to use MPI to run Island Model if it's required (e.g. on a
 * cluster).
 */
// $Log$
#include "gaim.h"
#include "mpi.h"


/**
 * @brief Structure containing information about MPI's communication graph for
 * the Island Model implementation.
 *
 * A structure that contains all the parameters for each node on the
 * Communication Graph of MPI.
 */
typedef struct node_struct {
    int ideg;        /**< Input degree of the node. */
    int odeg;        /**< Output degree of the node. */
    int *srcs;       /**< Sources of the node. */
    int *dsts;       /**< Destinations of the node. */
} node_s;


/**
 * @brief Reads the connectivity graph from file and builds the corresponding
 * communication graph used by MPI. 
 *
 *  Reads the connectivity graph (topology) from a text file. The graph defines
 *  the number of islands (run on separate threads) and how they are connected. 
 *  The format of the file is as follows:
 *  XX (number of islands - vertices)
 *  Node (Rank) #income_edges #outcome_edges INED1 INED2, ... INEDN, OUTED1, ..., OUTEDN
 *
 *  Example (ring topology with four islands):
 *  4
 *  0 1 1 3 1
 *  1 1 1 0 2 
 *  2 1 1 1 3 
 *  3 1 1 2 0
 *
 * It initializes all the necessary parameters and vectors for evolving a 
 * population of individuals based on a predetermined Genetic Algorithm (GA). 
 *
 * @param[in] fname         Name of the file contains the adjacent graph
 * @param[in] num_islands   Number of islands (MPI processes)
 * @return A C++ map that maps the number of an MPI process with the
 * destination processes.
 */
// !FIXME There is a potential memory leak issue in this function
std::map<int, node_s> read_connectivity_graph(std::string fname,
                                              std::size_t num_islands)
{
    int node, content;
    std::size_t num_vertices, num_in_edges, num_out_edges;
    node_s tmp;
    std::map<int, node_s> adj_list;   // Adjacent list of islands

    auto ifile = std::fstream(fname, std::ios::in);
    if (!ifile) {
        std::cout << "Unable to open file " << fname << std::endl;
        exit(1);
    }

    ifile >> num_vertices;
    for (size_t i = 0; i < num_vertices; ++i) {
        ifile >> node;
        ifile >> num_in_edges;
        ifile >> num_out_edges;
        tmp.odeg = num_out_edges;
        tmp.ideg = num_in_edges;

        tmp.srcs = (int *)malloc(sizeof(int) * tmp.ideg);
        for (size_t j = 0; j < num_in_edges; ++j) {
            ifile >> content;
            if (content < 0 && content > (int) num_islands) {
                std::cerr << "Error: Invalid source island!" << std::endl;
                exit(-1);
            }
            tmp.srcs[j] = content;
        }

        tmp.dsts = (int *)malloc(sizeof(int) * tmp.odeg);
        for (size_t j = 0; j < num_out_edges; ++j) {
            ifile >> content;
            if (content < 0 && content > (int) num_islands) {
                std::cerr << "Error: Invalid destination island!" << std::endl;
                exit(-1);
            }
            tmp.dsts[j] = content;
        }

        adj_list.emplace(std::make_pair(node, tmp));
        // free(tmp.dsts);
    }
    ifile.close();
    return adj_list;
}


/**
 * @brief Chooses the immigrants to from each island (MPI process)
 *
 * It takes as argument the buffer that will be populated with individuals
 * as potential migrants.
 * It supports three methods of distribution. random - where individuals are
 * randomly chosen, poor - individuals with the lowest fitness are chosen, 
 * and finally elit - where individuals with the best fitness are chosen.
 *
 * @param[in] pool Is the vector of indviduals within an Island (MPI process) 
 * @param[in] buffer A buffer array that is populated with potential migrants
 * @param[in] (*f) A ointer to a function (fitness)
 * @param[in] a (float) The lower bound of fitness function domain
 * @param[in] b (float) The upper bound of fitness function domain
 * @param[in] num_immigrants Number of individuals to be chosen
 * @param[in] genome_size Size of individuals genome
 * @param[in] method   A string that describes the method of replacement
 * @return Nothing
 */
void select_ind2migrate(std::vector<individual_s> pool,
                        REAL_ **buffer,
                        // REAL_ (*f)(std::vector<REAL_> &),
                        std::function<REAL_(std::vector<REAL_>&)> f,
                        std::vector<REAL_> a,
                        std::vector<REAL_> b,
                        std::size_t num_immigrants,
                        std::size_t genome_size,
                        std::string method)
{
    std::size_t id;
    std::size_t len = pool.size();
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> probs(a[0], b[0]);
    std::vector <REAL_> new_genome(genome_size);
    static std::vector<int> pop(pool.size());

    std::iota(std::begin(pop), std::end(pop), 0);
    std::shuffle(std::begin(pop), std::end(pop), gen);

    if (method == "random") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            id = pop[i];
            std::copy(pool[id].genome.begin(),
                      pool[id].genome.end(), 
                      *buffer + (i*genome_size));
            std::generate(new_genome.begin(),
                          new_genome.end(),
                          [&]{return probs(gen);});
            pool[id].genome = new_genome;
            pool[id].fitness = f(new_genome);
        }
    } else if (method == "elit") {
        for (size_t i = len-1, j = 0; j < num_immigrants; --i, ++j) {
            std::copy(pool[i].genome.begin(),
                      pool[i].genome.end(),
                      *buffer + (j*genome_size));
            std::generate(new_genome.begin(),
                          new_genome.end(),
                          [&]{return probs(gen);});
            pool[i].genome = new_genome;
            pool[i].fitness = f(new_genome);
        }
    } else if (method == "poor") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            std::copy(pool[i].genome.begin(),
                      pool[i].genome.end(),
                      *buffer + (i*genome_size));
            std::generate(new_genome.begin(),
                          new_genome.end(),
                          [&]{return probs(gen);});
            pool[i].genome = new_genome;
            pool[i].fitness = f(new_genome);
        }
    } else {
        std::cerr << "No such immigration method exists!" <<std::endl;
        exit(-1);
    }
}


/**
 * @brief Distributes the immigrants to their destination MPI processes. 
 *
 * It takes as argument the buffer containing all the migrants and distributes
 * them to their final destination MPI process. It supports three methods of 
 * distribution. random - where random individuals from within the local
 * pooulation are being replaced, poor - where individuals with the lowest
 * fitness are replaced, and finally elit - where individuals with the best 
 * fitness are replaced by migrants.
 *
 * @param[in] pool Is the vector of indviduals within an Island (MPI process) 
 * @param[in] buffer A buffer array that contains the migrants
 * @param[in] num_immigrants Number of individuals to be replaced
 * @param[in] genome_size Size of individuals genome
 * @param[in] method   A string that describes the method of replacement
 * @return Nothing
 */
void receiving_immigrants(std::vector<individual_s> pool,
                          REAL_ *buffer,
                          // REAL_ (*f)(std::vector<REAL_> &),
                          std::function<REAL_(std::vector<REAL_>&)> f,
                          std::size_t num_immigrants,
                          std::size_t genome_size,
                          std::string method)
{
    std::size_t id;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<individual_s> tmp(num_immigrants);
    static std::vector<int> pop(pool.size());

    std::iota(std::begin(pop), std::end(pop), 0);
    std::shuffle(std::begin(pop), std::end(pop), gen);

    if (method == "random") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            id = pop[i];
            std::copy(buffer+(i*genome_size),
                      buffer+((i+1)*genome_size),
                      pool[id].genome.begin());
            pool[id].fitness = f(pool[id].genome);
        }
    } else if (method == "poor") {
        for (size_t i = 0; i < num_immigrants; ++i) {
            std::copy(buffer+(i*genome_size),
                      buffer+((i+1)*genome_size),
                      pool[i].genome.begin());
            pool[i].fitness = f(pool[i].genome);
        }
    } else if (method == "elit") {
        id = pool.size();
        for (size_t i = 0; i < num_immigrants; ++i) {
            std::copy(buffer+(i*genome_size),
                      buffer+((i+1)*genome_size),
                      pool[id-1-i].genome.begin());
            pool[id-1-i].fitness = f(pool[id-1-i].genome);
        }
    } else {
        std::cerr << "No such immigration method exists!" <<std::endl;
        exit(-1);
    }
}


/**
 * @brief Print average fitness, BSF, best genome to files.
 *
 * It takes as argument the population structure, the Best-so-Far (BSF) 
 * fitness, the best-so-far genome and the average fitness of the entire
 * poputlation and it plots them to the corresponding files. 
 *
 * @param[in] population Is the vector of indviduals within an Island (MPI process) 
 * @param[in] bsf A vector of type REAL_ with the Best-so-Far fitness
 * @param[in] genome A vector of type REAL_ with the best-so-far genome
 * @param[in] fit_avg Is the vector of average fitness of the entire population
 * @param[in] unique_id  It is the rank of the MPI process that calls this function
 * @param[in] pms A pointer to the logging parameters data structure
 * @return Nothing
 */
void print_results(std::vector<individual_s> population,
                   std::vector<REAL_> bsf,
                   std::vector<REAL_> genome,
                   std::vector<REAL_> fit_avg,
                   std::size_t unique_id,
                   pr_parameter_s *pms)
{
    if (pms->print_fitness) {
        print_fitness(population, unique_id, pms->where2write);
    }
    if (pms->print_average_fitness) {
        print_avg_fitness(fit_avg, unique_id, pms->where2write);
    }
    if (pms->print_bsf) {
        print_bsf(bsf, unique_id, pms->where2write);
    } 
    if (pms->print_best_genome) {
        print_best_genome(genome, unique_id, pms->where2write);
    }
}


int main(int argc, char **argv)
{
    int rank, size, flag;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Get the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the size of the world
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ideg, odeg, wgt;
    int *in = NULL, *out = NULL;
    int *dsts = NULL;
    size_t data_size;

    // Define all GAIM parameters structures
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;
    std::tuple<ga_parameter_s, pr_parameter_s, im_parameter_s> pms;

    // Define the map structure for the connectivity graph (Adj Mat)
    std::map<int, node_s> adj_list;
    
    // Load all the necessary parameters from the cfg file
    std::tie(ga_pms, pr_pms, im_pms) = read_parameters_file("demo_island_model.cfg");
    print_parameters(ga_pms, pr_pms, im_pms);

    // Number of data get transfered
    data_size = im_pms.num_immigrants*ga_pms.genome_size;
    
    // Compute the migration steps
    size_t migration_steps = ga_pms.generations / im_pms.migration_interval;
    
    if ((size_t)size != im_pms.num_islands) {
        std::cerr << "Number of Islands mistmatches MPI rank size!" << std::endl;
        exit(-1);
    }

    MPI_Comm comm_dist_graph;
    
    // Read the connectivity graph from a file
    adj_list = read_connectivity_graph(im_pms.adj_list_fname,
                                       im_pms.num_islands);

    REAL_ **sbuf = (REAL_ **)malloc(sizeof(REAL_ *) * im_pms.num_islands);
    REAL_ **rbuf = (REAL_ **)malloc(sizeof(REAL_ *) * im_pms.num_islands);
    for (size_t i = 0; i < im_pms.num_islands; ++i) {
        sbuf[i] = (REAL_ *)calloc(data_size, sizeof(REAL_));
        rbuf[i] = (REAL_ *)calloc(data_size, sizeof(REAL_));
    }

    // Define islands topology and determine the MPI graph
    flag = MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, adj_list[rank].ideg, 
                                          adj_list[rank].srcs, MPI_UNWEIGHTED,
                                          adj_list[rank].odeg, adj_list[rank].dsts, 
                                          MPI_UNWEIGHTED, MPI_INFO_NULL, 1,
                                          &comm_dist_graph);

    // Get neighbors count 
    MPI_Dist_graph_neighbors_count(comm_dist_graph, &ideg, &odeg, &wgt);

    in = (int *)malloc(ideg * sizeof(int));
    out = (int *)malloc(odeg * sizeof(int));

    // Get neighbors dist
    MPI_Dist_graph_neighbors(comm_dist_graph, ideg, in, MPI_UNWEIGHTED,
                             odeg, out, MPI_UNWEIGHTED);

    // Instantiate the GA class
    GA gen_alg(&ga_pms);

    // Define the fitness function
    gen_alg.fitness = &sphere;

    // Evaluate individuals 
    gen_alg.evaluation(gen_alg.population);

    MPI_Request *rreqs, *sreqs;
    rreqs = (MPI_Request *)malloc(sizeof(MPI_Request) * ideg);
    sreqs = (MPI_Request *)malloc(sizeof(MPI_Request) * odeg);
    for (size_t i = 0; i < ga_pms.generations; ++i) {
        // Run an evolution step on each island
        gen_alg.run_one_generation();
        
        // Migration takes place
        if (i % im_pms.migration_interval == 0) {
            // Pick up the candidate immigrants
            select_ind2migrate(gen_alg.sorted_population,
                               &sbuf[rank],
                               gen_alg.fitness,
                               ga_pms.a,
                               ga_pms.b,
                               im_pms.num_immigrants,
                               ga_pms.genome_size,
                               im_pms.pick_method);
            // Send immigrants
            for (int j = 0; j < odeg; ++j) {
                MPI_Isend(&sbuf[rank], data_size, MPI_FLOAT, out[j], 99,
                          comm_dist_graph, &sreqs[j]);
            }

            MPI_Barrier(comm_dist_graph);
            
            // Receive immigrants
            for (int j = 0; j < ideg; ++j) {
                MPI_Irecv(rbuf[rank], data_size, MPI_FLOAT, in[j], 99,
                          comm_dist_graph, &rreqs[j]);
            }

            // Wait for all the process to move information
            MPI_Waitall(ideg, rreqs, MPI_STATUSES_IGNORE);
            MPI_Waitall(odeg, sreqs, MPI_STATUSES_IGNORE);

            // Move immigrants
            receiving_immigrants(gen_alg.sorted_population,
                                 rbuf[rank],
                                 gen_alg.fitness,
                                 im_pms.num_immigrants,
                                 ga_pms.genome_size,
                                 im_pms.replace_method);
        }
        MPI_Barrier(comm_dist_graph);
    }

    // Print all the necessary results in the appropriate files
    print_results(gen_alg.population,
                  gen_alg.bsf,
                  gen_alg.bsf_genome,
                  gen_alg.fit_avg,
                  rank,
                  &pr_pms);
    
    for (size_t i = 0; i < 0; ++i) {
        free(rbuf[i]);
        free(sbuf[i]);
    }
    free(rbuf);
    free(sbuf);
    free(in);
    free(out);
    free(dsts);
    free(rreqs);
    free(sreqs);
    
    // Finalize MPI
    MPI_Finalize();
}
