/* Logging cpp file for GAIM software 
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
 * @file logging.cpp
 * Manages serialization of fitness (avg or BSF) and genomes, as well as providing prettyprinting.  
 */
// $Log$
#include "gaim.h"


/**
 *  Converts a number to a string with specific format and tabs.
 *
 *  @param[in] x Number to be converted to string
 *  @param[in] decDigits Number of decimal digits to be used
 *  @param[in] width Width of the string
 *
 *  @return A string that contains the number x
 */
std::string prd(const REAL_ x, const int decDigits, const int width) {
    std::stringstream ss;
    ss.fill(' ');
    ss.width(width);
    ss.precision(decDigits);
    ss << x;
    return ss.str();
}


/**
 * Centers a string so it can be printed in a pretty way. 
 *
 * @param[in] s The input string to be centered
 * @param[in] w The width of the total string
 *
 * @return A string that is centered and padded with spaces
 */
std::string center(const std::string s, const int w) {
    std::stringstream ss, spaces;
    int padding = w - s.size();
    for(int i=0; i<padding/2; ++i)
        spaces << " ";
    ss << spaces.str() << s << spaces.str();
    if(padding > 0 && padding % 2 !=0)
        ss << " ";
    return ss.str();
}


/**
 * Prints the fitness of each individual within a population, as well as its
 * ID, either to standard output (stdout) or to a file. 
 *
 * @param[in] individual The population vector
 * @param[in] unique_id This is the thread id in case the Island Model is used;
 *                      otherwise it can be any number of type size_t
 * @param[in] write_to  A string that defines where the fitness will be
 *                      displayed ("stdout" or "/path/to/file")
 * 
 * @return Nothing. (void)
 */
void print_fitness(std::vector<individual_s> &individual,
                   std::size_t unique_id,
                   std::string write_to)
{
    std::string id, fit;
    std::stringstream ss;

    if (write_to == "stdout") {
        std::cout << center(" ID", 5) << " | " << center(" Fitness", 10) << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        for (auto &p : individual) {
            std::cout << std::setw(5) << p.id  << " | " << prd(p.fitness, 7, 10) << std::endl;
        }
    } else {
        if (!is_path_exist(write_to)) {
            std::cerr << "The directory "+write_to+" does not exist!" << std::endl;
            std::cerr << "Nothing will be saved!" << std::endl;
        } else {
            std::string fname = write_to+"fitness_";
            auto ofile = std::fstream(fname+std::to_string(unique_id)+".dat",
                                      std::ios::out | std::ios::binary);
            std::cout << "Writing the fitness to a file." << std::endl;
            for (auto &p : individual) {
                ofile.write((char *) &p.id, sizeof(int));
                ofile.write((char *) &p.fitness, sizeof(REAL_));
            }
            ofile.close();
        }
    }
}


/**
 * Prints the best-so-far fitness (BSF) of a population either to standard 
 * output (stdout) or to a file. 
 *
 * @param[in] bsf Best-so-far individual's fitness over generations
 * @param[in] unique_id This is the thread id in case the Island Model is used
 *                      otherwise it can be any number of type size_t
 * @param[in] write_to  A string that defines where the fitness will be
 *                      displayed ("stdout" or "/path/to/file")
 * 
 * @return Nothing. (void)
 */
void print_bsf(std::vector<REAL_> bsf,
               std::size_t unique_id,
               std::string write_to) {
    std::size_t iter(0);
    
    if (write_to == "stdout") {
        std::cout << center("Generation #", 12) << " | " << center("BSF", 10) << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        for (auto &b : bsf) {
            std::cout << std::setw(12) << iter << " | " << prd(b, 7, 4) << std::endl;
            iter++;
        }
    }else{
        if (!is_path_exist(write_to)) {
            std::cerr << "The directory "+write_to+" does not exist!" << std::endl;
            std::cerr << "Nothing will be saved!" << std::endl;
        } else {
            std::string fname = write_to+"bsf_";
            auto ofile = std::fstream(fname+std::to_string(unique_id)+".dat",
                                      std::ios::out | std::ios::binary);
            std::cout << "Writing the BSF fitness to a file." << std::endl;
            for (auto &b : bsf) {
                ofile.write((char *) &b, sizeof(REAL_));
            }
            ofile.close();
        }
    }
}


/**
 * Prints the average fitness of a population either to standard output 
 * (stdout) or to a file. 
 *
 * @param[in] fit_avg Average fitness of a population over generations
 * @param[in] unique_id This is the thread id in case the Island Model is used
 *                      otherwise it can be any number of type size_t
 * @param[in] write_to  A string that defines where the fitness will be
 *                      displayed ("stdout" or "/path/to/file")
 * 
 * @return Nothing. (void)
 */
void print_avg_fitness(std::vector<REAL_> fit_avg,
                       std::size_t unique_id, 
                       std::string write_to) {
    std::size_t iter(0);

    if (write_to == "stdout") {
        std::cout << center("Generation #", 12) << " | " << center("Average Fitness", 10) << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        for (auto &b : fit_avg) {
            std::cout << std::setw(12) << iter << " | " << prd(b, 7, 4) << std::endl;
            iter++;
        }
    }else{
        if (!is_path_exist(write_to)) {
            std::cerr << "The directory "+write_to+" does not exist!" << std::endl;
            std::cerr << "Nothing will be saved!" << std::endl;
        } else {
            std::string fname = write_to+"average_fitness_";
            auto ofile = std::fstream(fname+std::to_string(unique_id)+".dat",
                                      std::ios::out | std::ios::binary);
            std::cout << "Writing the average fitness to a file." << std::endl;
            for (auto &a : fit_avg) {
                ofile.write((char *) &a, sizeof(REAL_));
            }
            ofile.close();
        }
    }
}


/**
 * Prints the genome of an individual of a population with the best fitness
 * either to standard output (stdout) or to a file. 
 *
 * @param[in] best_genome Vector of the best genome within a population
 * @param[in] unique_id This is the thread id in case the Island Model is used
 *                      otherwise it can be any number of type size_t
 * @param[in] write_to  A string that defines where the fitness will be
 *                      displayed ("stdout" or "/path/to/file")
 * 
 * @return Nothing. (void)
 */
void print_best_genome(std::vector<REAL_> best_genome,
                       std::size_t unique_id,
                       std::string write_to)
{
    if (write_to == "stdout") {
        std::cout << std::endl;
        std::cout << "Best Genome" << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        for (auto &g : best_genome) {
            std::cout << g << "  ";
        }
        std::cout << std::endl;
    }else{
        if (!is_path_exist(write_to)) {
            std::cerr << "The directory "+write_to+" does not exist!" << std::endl;
            std::cerr << "Nothing will be saved!" << std::endl;
        } else {
            std::string fname = write_to+"best_genome_";
            auto ofile = std::fstream(fname+std::to_string(unique_id)+".dat",
                                      std::ios::out | std::ios::binary);
            std::cout << "Writing the best individual's genome to a file." << std::endl;
            for (auto &g : best_genome) {
                ofile.write((char *) &g, sizeof(REAL_));
            }
            ofile.close();
        }
    }
}
