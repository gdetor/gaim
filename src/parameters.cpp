/* Parameters cpp file for GAIM software 
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
 * @file parameters.cpp
 * Provides I/O of GAIM configuration files holding GA, IM, and printing/debug
 * parameters.
 */
// $Log$
#include "gaim.h"

using namespace libconfig;


/**
 * @brief Reads GAIM config files containing Genetic Algorithm, Island Model 
 * and printing parameters.
 *
 * Reads a configuration file (.cfg) of libconfig format, parses it and loads
 * the parameters into the internal data structures. 
 *
 * @param[in] fname Configuration filename (libconfig format).
 * @return A tuple of three structures (parameter_s, print_params_s,
 *         im_parameter_s) -> (GA, printing, IM).
 */
std::tuple<ga_parameter_s,
           pr_parameter_s,
           im_parameter_s> read_parameters_file(const char *fname)
{
    Config cfg;
    ga_parameter_s tmp;
    pr_parameter_s print_tmp;
    im_parameter_s island_tmp;

    int generations, population_size, num_offsprings, genome_size, k;
    int num_replacement, runs;
    int num_islands, num_immigrants, migration_interval;
    int flag, time, order;
    int num_parents, lower_bound;
    REAL_ bias, mutation_rate, variance;
    REAL_ low_bound, up_bound;
    bool print_fitness, print_avg_fitness, print_bsf, print_best_genome;
    bool im_enabled, replace, is_real;
    std::string clipping, method;
    std::string where2write, exp_name, final_path, adj_list_fname;
    std::string base("./"), rmethod, cmethod;
    std::string clipping_fname;

    // Read the file. If there is an error, report it and exit.
    try
        {
            cfg.readFile(fname);
        }
    catch(const FileIOException &fioex)
        {
            std::cerr << "I/O error while reading file." << std::endl;
            exit(-1);
        }
    catch(const ParseException &pex)
        {
            std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                      << " - " << pex.getError() << std::endl;
            exit(-1);
        }

    // Get the simulation name.
    try
        {
            std::string name = cfg.lookup("name");
            exp_name = name;
            std::cout << "Simulation: " << name << std::endl << std::endl;
        }
    catch(const SettingNotFoundException &nfex)
        {
            std::cerr << "No 'name' setting in configuration file." << std::endl;
        }

    const Setting& root = cfg.getRoot();
    try
        {
            const Setting &ga = root["GA"]["evolution"];
            const Setting &sel = root["GA"]["selection"];
            const Setting &cross = root["GA"]["crossover"];
            const Setting &mut = root["GA"]["mutation"];
            const Setting &pr = root["GA"]["print"];
            const Setting &im = root["GA"]["island_model"];

            const Setting &a_cfg = ga.lookup("a");
            std::vector<REAL_> tmp_a;
            for (int n = 0; n < a_cfg.getLength(); ++n) {
                tmp_a.push_back(a_cfg[n]);
            }

            const Setting &b_cfg = ga.lookup("b");
            std::vector<REAL_> tmp_b;
            for (int n = 0; n < b_cfg.getLength(); ++n) {
                tmp_b.push_back(b_cfg[n]);
            }

            // GA parameters
            if (ga.lookupValue("generations", generations) &&
                ga.lookupValue("runs", runs) &&
                ga.lookupValue("population_size", population_size) &&
                ga.lookupValue("number_of_offsprings", num_offsprings) &&
                ga.lookupValue("size_of_genome", genome_size) &&
                ga.lookupValue("clipping", clipping) &&
                ga.lookupValue("clipping_fname", clipping_fname) &&
                ga.lookupValue("number_of_replacement", num_replacement)) {
            
                if ((generations < 0) || (population_size <= 0) ||
                        (num_offsprings <= 0) || (genome_size <= 0) || 
                        (num_replacement < 0)){
                    std::cerr << "Negative parameters detected!" << std::endl;
                    exit(-1);
                } 

                tmp.runs = runs;
                tmp.generations = generations;
                tmp.population_size = population_size;
                tmp.num_offsprings = num_offsprings;
                if (num_offsprings >= population_size) {
                    std::cerr << "Number of offsprings cannot exceed population size!" << std::endl;
                    exit(-1);
                }
                tmp.genome_size = genome_size;
                if (genome_size < 1) { 
                    std::cerr << "Genome size cannot be smaller than 1!" << std::endl;
                    exit(-1);
                }
                tmp.num_replacement = num_replacement;
                tmp.clipping = clipping;
                tmp.clipping_fname = clipping_fname;
            }
            tmp.a = tmp_a;
            tmp.b = tmp_b;
            
            // Selection parameters
            if (sel.lookupValue("selection_method", method) &&
                sel.lookupValue("bias", bias) &&
                sel.lookupValue("num_parents", num_parents) &&
                sel.lookupValue("lower_bound", lower_bound) &&
                sel.lookupValue("k", k) &&
                sel.lookupValue("replace", replace))  {
            
                tmp.sel_pms.selection_method = method;
                tmp.sel_pms.bias = bias;
                tmp.sel_pms.num_parents = num_parents;
                tmp.sel_pms.lower_bound = lower_bound;
                tmp.sel_pms.k = k;
                tmp.sel_pms.replace = replace;
            }

            // Crossover parameters
            if (cross.lookupValue("crossover_method", method)) {
                tmp.cross_pms.crossover_method = method;
            }

            // Selection parameters
            if (mut.lookupValue("mutation_method", method) &&
                mut.lookupValue("mutation_rate", mutation_rate) &&
                mut.lookupValue("variance", variance) &&
                mut.lookupValue("low_bound", low_bound) &&
                mut.lookupValue("up_bound", up_bound) &&
                mut.lookupValue("time", time) &&
                mut.lookupValue("order", order) &&
                mut.lookupValue("is_real", is_real))  {

                tmp.mut_pms.mutation_method = method;
                tmp.mut_pms.mutation_rate = mutation_rate;
                tmp.mut_pms.variance = variance;
                tmp.mut_pms.low_bound = low_bound;
                tmp.mut_pms.up_bound = up_bound;
                tmp.mut_pms.time = time;
                tmp.mut_pms.order = order;
                tmp.mut_pms.is_real = is_real;
            }

            // Printing parameters
            if (pr.lookupValue("print_fitness", print_fitness) &&
                pr.lookupValue("print_average_fitness", print_avg_fitness) &&
                pr.lookupValue("print_bsf", print_bsf) &&
                pr.lookupValue("print_best_genome", print_best_genome) &&
                pr.lookupValue("where_to_write", where2write))  {
            
                print_tmp.print_fitness = print_fitness;
                print_tmp.print_average_fitness = print_avg_fitness;
                print_tmp.print_bsf = print_bsf;
                print_tmp.print_best_genome = print_best_genome;
                print_tmp.where2write = where2write;
                print_tmp.experiment_name = exp_name;

            }

            // IM parameters 
            if (im.lookupValue("im_enabled", im_enabled) &&
                im.lookupValue("number_of_immigrants", num_immigrants) &&
                im.lookupValue("migration_interval", migration_interval) &&
                im.lookupValue("number_of_islands", num_islands) &&
                im.lookupValue("island_model_graph_file", adj_list_fname) &&
                im.lookupValue("candidate_immigrants_replace_method", rmethod) &&
                im.lookupValue("candidate_immigrants_pickup_method", cmethod)) {
            
                island_tmp.num_immigrants = num_immigrants;
                island_tmp.num_islands = num_islands;
                island_tmp.migration_interval = migration_interval;
                island_tmp.pick_method = cmethod;
                island_tmp.replace_method = rmethod;
                island_tmp.is_im_enabled = im_enabled;
                island_tmp.adj_list_fname = adj_list_fname;
            }

            if (island_tmp.is_im_enabled && tmp.runs > 1) {
                std::cerr << "Is not allowed to use more than 1 runs when IM is enabled!" << std::endl;
                exit(-1);
            } 

            if (island_tmp.is_im_enabled) {
                if (print_tmp.where2write == "stdout") {
                    print_tmp.where2write = "./results";
                }
            }

            if (print_tmp.where2write != "stdout") {
                if (!is_path_exist(print_tmp.where2write)) {
                    flag = mkdir_(print_tmp.where2write);
                    if (flag) {
                        exit(-1);
                    }
                }
            }
        }
        catch(const SettingNotFoundException &nfex)
        {
            //Ignore
        }
    return std::make_tuple(tmp, print_tmp, island_tmp);
}   


/**
 *  @brief Prints parameters to a .dat file, or to stdout if "stdout" is specified.
 *
 *  Prints all the parameters to the standard output (if "stdout" is specified
 *  as where2write in the printing params) and to a file for debugging purposes. 
 *
 * @param[in] ga_pms Structure of GA parameters
 * @param[in] pr_pms Structure of printing parameters
 * @param[in] im_pms Structure of IM parameters
 *
 * @return Nothing (void function)
 */
void print_parameters(ga_parameter_s ga_pms,
                      pr_parameter_s pr_pms,
                      im_parameter_s im_pms) 
{
    if (pr_pms.where2write == "stdout") {
        std::cout << "" << std::endl;
        std::cout << std::string(20, '*') << std::endl;
        std::cout << pr_pms.experiment_name << std::endl;
        std::cout << std::string(20, '*') << std::endl;
        std::cout << "Indepenent Runs: " << ga_pms.runs << std::endl;
        std::cout << "#Generations: " << ga_pms.generations << std::endl;
        std::cout << "#Individuals: " << ga_pms.population_size << std::endl;
        std::cout << "Genome Size: " << ga_pms.genome_size << std::endl;
        std::cout << "#Offsprings: " << ga_pms.num_offsprings << std::endl;
        std::cout << "#Replacements: " << ga_pms.num_replacement << std::endl;
        std::cout << "Clipping: " << ga_pms.clipping << std::endl;
        std::cout << "Clipping Values File: " << ga_pms.clipping_fname
            << std::endl;
        std::cout << "Selection method: " << ga_pms.sel_pms.selection_method
            << std::endl;
        std::cout << "Selection bias: " << ga_pms.sel_pms.bias << std::endl;
        std::cout << "Selection number of parents: "
            << ga_pms.sel_pms.num_parents << std::endl;
        std::cout << "Selection lower bound: " << ga_pms.sel_pms.lower_bound
            << std::endl;
        std::cout << "Selection k: " << ga_pms.sel_pms.k << std::endl;
        std::cout << "Selection replace: " << ga_pms.sel_pms.replace
            << std::endl;
        std::cout << "Crossover method: " << ga_pms.cross_pms.crossover_method
            << std::endl;
        std::cout << "Mutation method: " << ga_pms.mut_pms.mutation_method
            << std::endl;
        std::cout << "Delta mutation rate: " << ga_pms.mut_pms.mutation_rate
            << std::endl;
        std::cout << "Denta mutation var: " << ga_pms.mut_pms.variance
            << std::endl;
        std::cout << "Interval lower limit: " << ga_pms.mut_pms.low_bound
            << std::endl;
        std::cout << "Interval upper limit: " << ga_pms.mut_pms.up_bound
            << std::endl;
        std::cout << "Non-uniform mutation time: " << ga_pms.mut_pms.time
            << std::endl;
        std::cout << "Non-uniform mutation order: " << ga_pms.mut_pms.order
            << std::endl;
        std::cout << "Fusion and Random mutation float/int switch: "
            << ga_pms.mut_pms.is_real << std::endl;

        std::cout << "Print fitness: " << pr_pms.print_fitness << std::endl;
        std::cout << "Print average fitness: " << pr_pms.print_average_fitness
            << std::endl;
        std::cout << "Print BSF: " << pr_pms.print_bsf << std::endl;
        std::cout << "Print best genome: " << pr_pms.print_best_genome
            << std::endl;
        std::cout << "Island Model is " << im_pms.is_im_enabled << std::endl;
        std::cout << "#Islands: " << im_pms.num_islands << std::endl;
        std::cout << "#Immigrants: " << im_pms.num_immigrants << std::endl;
        std::cout << "Migration Interval: " << im_pms.migration_interval
            << std::endl;
        std::cout << "Choose immigrants method: " << im_pms.pick_method
            << std::endl;
        std::cout << "Replace individuals method: " << im_pms.replace_method
            << std::endl;
        std::cout << std::string(20, '*') << std::endl;
        std::cout << "" << std::endl;
    }else{
        auto ofile = std::fstream(pr_pms.where2write+pr_pms.experiment_name+".dat",
                                  std::ios::out);
        ofile << "" << std::endl;
        ofile << std::string(20, '*') << std::endl;
        ofile << pr_pms.experiment_name << std::endl;
        ofile << std::string(20, '*') << std::endl;
        ofile << "Indepenent Runs: " << ga_pms.runs << std::endl;
        ofile << "#Generations: " << ga_pms.generations << std::endl;
        ofile << "#Individuals: " << ga_pms.population_size << std::endl;
        ofile << "Genome Size: " << ga_pms.genome_size << std::endl;
        ofile << "#Offsprings: " << ga_pms.num_offsprings << std::endl;
        ofile << "#Replacements: " << ga_pms.num_replacement << std::endl;
        ofile << "Clipping: " << ga_pms.clipping << std::endl;
        ofile << "Clipping Values File: " << ga_pms.clipping_fname << std::endl;
        ofile << "Selection method: " << ga_pms.sel_pms.selection_method
            << std::endl;
        ofile << "Selection bias: " << ga_pms.sel_pms.bias << std::endl;
        ofile << "Selection number of parents: " << ga_pms.sel_pms.num_parents
            << std::endl;
        ofile << "Selection lower bound: " << ga_pms.sel_pms.lower_bound
            << std::endl;
        ofile << "Selection k: " << ga_pms.sel_pms.k << std::endl;
        ofile << "Selection replace: " << ga_pms.sel_pms.replace << std::endl;
        ofile << "Crossover method: " << ga_pms.cross_pms.crossover_method
            << std::endl;
        ofile << "Mutation method: " << ga_pms.mut_pms.mutation_method
            << std::endl;
        ofile << "Delta mutation rate: " << ga_pms.mut_pms.mutation_rate
            << std::endl;
        ofile << "Denta mutation var: " << ga_pms.mut_pms.variance
            << std::endl;
        ofile << "Interval lower limit: " << ga_pms.mut_pms.low_bound
            << std::endl;
        ofile << "Interval upper limit: " << ga_pms.mut_pms.up_bound
            << std::endl;
        ofile << "Non-uniform mutation time: " << ga_pms.mut_pms.time
            << std::endl;
        ofile << "Non-uniform mutation order: " << ga_pms.mut_pms.order
            << std::endl;
        ofile << "Fusion and Random mutation float/int switch: "
            << ga_pms.mut_pms.is_real << std::endl;
        ofile << "Print fitness: " << pr_pms.print_fitness << std::endl;
        ofile << "Print average fitness: " << pr_pms.print_average_fitness
            << std::endl;
        ofile << "Print BSF: " << pr_pms.print_bsf << std::endl;
        ofile << "Print best genome: " << pr_pms.print_best_genome
            << std::endl;
        ofile << "Island Model is " << im_pms.is_im_enabled << std::endl;
        ofile << "#Islands: " << im_pms.num_islands << std::endl;
        ofile << "#Immigrants: " << im_pms.num_immigrants << std::endl;
        ofile << "Migration Interval: " << im_pms.migration_interval
            << std::endl;
        ofile << "Choose immigrants method: " << im_pms.pick_method
            << std::endl;
        ofile << "Replace individuals method: " << im_pms.replace_method
            << std::endl;
        ofile << std::string(20, '*') << std::endl;
        ofile << "" << std::endl;
        ofile.close();
    }
}
