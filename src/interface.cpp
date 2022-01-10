#include "gaim.h"


int ga_optimization(REAL_ (*func)(std::vector<REAL_> &),
                    size_t n_generations,
                    size_t population_size,
                    size_t genome_size,
                    size_t n_offsprings,
                    size_t n_replacements,
                    size_t n_rounds,
                    size_t n_islands,
                    size_t n_immigrants,
                    size_t migration_interval,
                    REAL_ a,
                    REAL_ b,
                    char *pickup_method,
                    char *replace_method,
                    char *clipping_fname,
                    char *log_fname,
                    char *experiment_id,
                    char *im_graph_fname,
                    bool log_fitness,
                    bool log_average_fitness,
                    bool log_bsf,
                    bool log_best_genome,
                    bool universal_clipping,
                    bool is_im_enabled) {
    ga_parameter_s ga_pms;
    pr_parameter_s pr_pms;
    im_parameter_s im_pms;

    // Assign values to GA parameters
    ga_pms.runs = n_rounds;
    ga_pms.generations = n_generations;
    ga_pms.population_size = population_size;
    ga_pms.num_offsprings = n_offsprings;
    if (n_offsprings >= population_size) {
        printf("Number of offsprings cannot exceed population size!\n");
        return 1;
    }
    ga_pms.genome_size = genome_size;
    if (genome_size < 1) { 
        printf("Genome size cannot be smaller than 1!\n");
        return 2;
    }
    ga_pms.num_replacement = n_replacements;
    ga_pms.universal_clipping = universal_clipping;
    ga_pms.clipping_fname = clipping_fname;
    ga_pms.a = a;
    ga_pms.b = b;

    // Assign values to logging parameters
    pr_pms.print_fitness = log_fitness;
    pr_pms.print_average_fitness = log_average_fitness;
    pr_pms.print_bsf = log_bsf;
    pr_pms.print_best_genome = log_best_genome;
    pr_pms.where2write = log_fname;
    pr_pms.experiment_name = experiment_id;

    if (is_im_enabled) {
        im_pms.num_immigrants = n_immigrants;
        im_pms.num_islands = n_islands;
        im_pms.migration_interval = migration_interval;
        im_pms.pick_method = pickup_method;
        im_pms.replace_method = replace_method;
        im_pms.is_im_enabled = is_im_enabled;
        im_pms.adj_list_fname = im_graph_fname;
    }

    print_parameters(ga_pms, pr_pms, im_pms);
    if (im_pms.is_im_enabled) {
        IM island_model(&im_pms, &ga_pms);
        printf("Optimizing using Island Model!\n");
        for (size_t i = 0; i < im_pms.num_islands; ++i) {
           island_model.island[i].fitness=func;
        }
        island_model.run_islands(&im_pms, &pr_pms);
    } else {
        if (ga_pms.runs == 1) {
            printf("Optimizing using a single GA!\n");
            GA gen_alg(&ga_pms);
            gen_alg.fitness = func;
            gen_alg.evolve(ga_pms.generations, 0, &pr_pms);
        } else if (ga_pms.runs > 1) {
            printf("Running %d independent GAs!\n", ga_pms.runs);
            independent_runs(&ga_pms, &pr_pms);
        } else {
            printf("Negative number of runs is illegal!\n");
            return 3;
        }
    }
    return 0;
}
