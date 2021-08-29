/* pyGAIM: A Python wrapper for the GAIM library (C++ implementation)
 * Copyright (C) 2020  Georgios Detorakis (gdetor@protonmail.com)
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

/* $Id$ */

/* @file pygaim.py */
/* Defines a Python module for the GA class of GAIM. The present implementation
 * is based entirely on pybind11. */

/* $Log$ */
#include "gaim.h"

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>


namespace py = pybind11;


PYBIND11_MODULE(pygaim, m)
{
    // Wrapper for GA class
    py::class_<GA>(m, "GA")
        .def(py::init([] (ga_parameter_s *pms){
                    GA *ga = new GA(pms);
                    return ga;
                    }))
        .def("pycallback", &GA::pycallback)
        .def("selection", &GA::selection)
        .def("crossover", &GA::crossover)
        .def("mutation", &GA::mutation)
        .def("evaluation", &GA::evaluation)
        .def("next_generation", &GA::next_generation)
        .def("sort_population", &GA::sort_population)
        .def("run_one_generation", &GA::run_one_generation)
        .def("evolve", &GA::evolve)
        .def("get_best_genome", &GA::get_bsf)
        .def("clip_genome", &GA::clip_genome)
        .def("reset_selection_flags", &GA::reset_selection_flags)
        .def("get_average_fitness_record", &GA::get_average_fitness_rec);

    // Wrapper for GA parameters data structure
    py::class_<ga_parameter_s>(m, "ga_parameter_s")
        .def(py::init([] (){ return new ga_parameter_s; }))
        .def_readwrite("a", &ga_parameter_s::a)
        .def_readwrite("b", &ga_parameter_s::b)
        .def_readwrite("generations", &ga_parameter_s::generations)
        .def_readwrite("population_size", &ga_parameter_s::population_size)
        .def_readwrite("genome_size", &ga_parameter_s::genome_size)
        .def_readwrite("num_offsprings", &ga_parameter_s::num_offsprings)
        .def_readwrite("num_replacement", &ga_parameter_s::num_replacement)
        .def_readwrite("universal_clipping", &ga_parameter_s::universal_clipping)
        .def_readwrite("clipping_fname", &ga_parameter_s::clipping_fname)
        .def_readwrite("runs", &ga_parameter_s::runs);

    // Wrapper for GA logger parameters data structure
    py::class_<pr_parameter_s>(m, "pr_parameter_s")
        .def(py::init([] (){ return new pr_parameter_s; }))
        .def_readwrite("where2write", &pr_parameter_s::where2write)
        .def_readwrite("experiment_name", &pr_parameter_s::experiment_name)
        .def_readwrite("print_fitness", &pr_parameter_s::print_fitness)
        .def_readwrite("print_average_fitness", &pr_parameter_s::print_average_fitness)
        .def_readwrite("print_bsf", &pr_parameter_s::print_bsf)
        .def_readwrite("print_best_genome", &pr_parameter_s::print_best_genome);

    py::class_<individual_s>(m, "individual_s")
        .def(py::init([] (){ return new individual_s; }))
        .def_readwrite("is_selected", &individual_s::is_selected)
        .def_readwrite("id", &individual_s::id)
        .def_readwrite("fitness", &individual_s::fitness)
        .def_readwrite("genome", &individual_s::genome)
        .def_readwrite("lower_limit", &individual_s::lower_limit)
        .def_readwrite("upper_limit", &individual_s::upper_limit);

    py::class_<im_parameter_s>(m, "im_parameter_s")
        .def(py::init([] (){ return new im_parameter_s; }))
        .def_readwrite("num_immigrants", &im_parameter_s::num_immigrants)
        .def_readwrite("num_islands", &im_parameter_s::num_islands)
        .def_readwrite("migration_interval", &im_parameter_s::migration_interval)
        .def_readwrite("pick_method", &im_parameter_s::pick_method)
        .def_readwrite("replace_method", &im_parameter_s::replace_method)
        .def_readwrite("adj_list_fname", &im_parameter_s::adj_list_fname)
        .def_readwrite("is_im_enabled", &im_parameter_s::is_im_enabled);

    // Wrapper for IM class
    py::class_<IM>(m, "IM")
        .def(py::init([] (im_parameter_s *im, ga_parameter_s *pms){
                    IM *islands = new IM(im, pms);
                    return islands;
                    }))
        .def("read_connectivity_graph", &IM::read_connectivity_graph)
        .def("select_ind2migrate", &IM::select_ind2migrate)
        .def("move_immigrants", &IM::move_immigrants)
        .def("evolve_island", &IM::evolve_island)
        .def("run_islands", &IM::run_islands);

    // Crossover functions
    m.def("one_point_crossover",
          &one_point_crossover,
          "One point crossover method");
    m.def("two_point_crossover",
          &two_point_crossover,
          "Two point crossover method");
    m.def("uniform_crossover",
          &uniform_crossover,
          "Uniform crossover method");
    m.def("flat_crossover",
          &flat_crossover,
          "Flat crossover method");
    m.def("discrete_crossover",
          &discrete_crossover,
          "Discrete crossover method");
    m.def("order_one_crossover",
          &order_one_crossover,
          "Order one crossover method");

    // Selection functions
    m.def("ktournament_selection",
          &ktournament_selection,
          "k-tournament selection method");
    m.def("truncation_selection",
          &truncation_selection,
          "Truncation selection method");
    m.def("linear_rank_selection",
          &linear_rank_selection,
          "Linear Rank selection method");
    m.def("random_selection",
          &random_selection,
          "Random selection method");
    m.def("roulette_wheel_selection",
          &roulette_wheel_selection,
          "Roulette wheel selection method");
    m.def("stochastic_roulette_wheel_selection",
          &stochastic_roulette_wheel_selection,
          "Roulette-wheel selection via stochastic acceptance");
    m.def("whitley_selection_method",
          &whitley_selection,
          "Whitley selection method");

    // Mutation functions
    m.def("delta_mutation",
          &delta_mutation,
          "Delta mutation method");
    m.def("random_mutation",
          &random_mutation,
          "Random mutation method");
    m.def("nonuniform_mutation",
          &nonuniform_mutation,
          "Non-uniform mutation method");
    m.def("fusion_mutation",
          &fusion_mutation,
          "Fusion mutation");
    m.def("swap_mutation",
          &swap_mutation,
          "Swap mutation method");
    m.def("whitley_selection",
          &whitley_selection,
          "Whitley rank selection method");
}
