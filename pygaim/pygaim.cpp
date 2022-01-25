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
        .def("ktournament_selection", &GA::ktournament_selection)
        .def("truncation_selection", &GA::truncation_selection)
        .def("linear_rank_selection", &GA::linear_rank_selection)
        .def("random_selection", &GA::random_selection)
        .def("roulette_wheel_selection", &GA::roulette_wheel_selection)
        .def("stochastic_roulette_wheel_selection", &GA::stochastic_roulette_wheel_selection)
        .def("whitley_selection", &GA::whitley_selection)
        .def("select_selection_method", &GA::select_selection_method)
        .def("one_point_crossover", &GA::one_point_crossover)
        .def("two_point_crossover", &GA::two_point_crossover)
        .def("uniform_crossover", &GA::uniform_crossover)
        .def("flat_crossover", &GA::flat_crossover)
        .def("discrete_crossover", &GA::discrete_crossover)
        .def("order_one_crossover", &GA::order_one_crossover)
        .def("select_crossover_method", &GA::select_crossover_method)
        .def("delta_mutation", &GA::delta_mutation)
        .def("random_mutation", &GA::random_mutation)
        .def("nonuniform_mutation", &GA::nonuniform_mutation)
        .def("fusion_mutation", &GA::fusion_mutation)
        .def("swap_mutation", &GA::swap_mutation)
        .def("select_mutation_method", &GA::select_mutation_method)
        .def("evaluation", &GA::evaluation)
        .def("next_generation", &GA::next_generation)
        .def("sort_population", &GA::sort_population)
        .def("run_one_generation", &GA::run_one_generation)
        .def("evolve", &GA::evolve)
        .def("get_best_genome", &GA::get_best_genome)
        .def("get_bsf", &GA::get_bsf)
        .def("clip_genome", &GA::clip_genome)
        .def("reset_selection_flags", &GA::reset_selection_flags)
        .def("get_average_fitness", &GA::get_average_fitness);

    py::class_<sel_parameter_s>(m, "sel_parameter_s")
        .def(py::init([] (){ return new sel_parameter_s; }))
        .def_readwrite("selection_method", &sel_parameter_s::selection_method)
        .def_readwrite("bias", &sel_parameter_s::bias)
        .def_readwrite("num_parents", &sel_parameter_s::num_parents)
        .def_readwrite("lower_bound", &sel_parameter_s::lower_bound)
        .def_readwrite("k", &sel_parameter_s::k)
        .def_readwrite("replace", &sel_parameter_s::replace);

    py::class_<cross_parameter_s>(m, "cross_parameter_s")
        .def(py::init([] (){ return new cross_parameter_s; }))
        .def_readwrite("crossover_method", &cross_parameter_s::crossover_method);

    py::class_<mut_parameter_s>(m, "mut_parameter_s")
        .def(py::init([] (){ return new mut_parameter_s; }))
        .def_readwrite("mutation_method", &mut_parameter_s::mutation_method)
        .def_readwrite("mutation_rate", &mut_parameter_s::mutation_rate)
        .def_readwrite("variance", &mut_parameter_s::variance)
        .def_readwrite("low_bound", &mut_parameter_s::low_bound)
        .def_readwrite("up_bound", &mut_parameter_s::up_bound)
        .def_readwrite("time", &mut_parameter_s::time)
        .def_readwrite("order", &mut_parameter_s::order)
        .def_readwrite("is_real", &mut_parameter_s::is_real);

    // Wrapper for GA parameters data structure
    py::class_<ga_parameter_s>(m, "ga_parameter_s")
        .def(py::init([] (){ return new ga_parameter_s; }))
        .def_readwrite("sel_pms", &ga_parameter_s::sel_pms)
        .def_readwrite("cross_pms", &ga_parameter_s::cross_pms)
        .def_readwrite("mut_pms", &ga_parameter_s::mut_pms)
        .def_readwrite("a", &ga_parameter_s::a)
        .def_readwrite("b", &ga_parameter_s::b)
        .def_readwrite("generations", &ga_parameter_s::generations)
        .def_readwrite("population_size", &ga_parameter_s::population_size)
        .def_readwrite("genome_size", &ga_parameter_s::genome_size)
        .def_readwrite("num_offsprings", &ga_parameter_s::num_offsprings)
        .def_readwrite("num_replacement", &ga_parameter_s::num_replacement)
        .def_readwrite("clipping", &ga_parameter_s::clipping)
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

    py::class_<py_results_s>(m, "py_results_s")
        .def(py::init([] (){ return new py_results_s; }))
        .def_readwrite("genome", &py_results_s::genome)
        .def_readwrite("bsf", &py_results_s::bsf)
        .def_readwrite("average_fitness", &py_results_s::average_fitness);

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

    // Interface function
    m.def("ga_optimization", &ga_optimization,
          py::arg("func"),
          py::arg("n_generations"),
          py::arg("population_size"),
          py::arg("genome_size"),
          py::arg("n_offsprings"),
          py::arg("n_replacements"),
          py::arg("n_rounds"),
          py::arg("a"),
          py::arg("b"),
          py::arg("clipping")="universal",
          py::arg("clipping_fname")="clamp_fname.dat",
          py::arg("selection_method")="ktournament",
          py::arg("bias")=1.5,
          py::arg("num_parents")=2,
          py::arg("lower_bound")=1,
          py::arg("k")=2,
          py::arg("replace")=false,
          py::arg("crossover_method")="one_point",
          py::arg("mutation_method")="delta",
          py::arg("mutation_rate")=0.5,
          py::arg("mutation_var")=0.5,
          py::arg("low_bound")=0.0,
          py::arg("up_bound")=1.0,
          py::arg("order")=1,
          py::arg("is_real")=true,
          py::arg("is_im_enabled")=false,
          py::arg("n_islands"),
          py::arg("n_immigrants"),
          py::arg("migration_interval"),
          py::arg("pickup_method")="elite",
          py::arg("replace_method")="poor",
          py::arg("im_graph_fname")="graph.dat",
          py::arg("experiment_id")="experiment1",
          py::arg("log_fname")="./data/",
          py::arg("log_fitness")=false,
          py::arg("log_average_fitness")=true,
          py::arg("log_bsf")=true,
          py::arg("log_best_genome")=true);
}
