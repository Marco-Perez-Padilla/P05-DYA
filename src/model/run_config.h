/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 24/04/2026

** Archivo run_config.h: Definición de la estructura RunConfig, que almacena toda la configuración de la ejecución
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef RUN_CONFIG_H
#define RUN_CONFIG_H

#include <vector>
#include <string>

enum class LSMode {
    NONE,
    ONLY_LS,
    BOTH
};

struct RunConfig {
    // Algoritmos a ejecutar
    bool run_greedy  = false;
    bool run_GRASP   = false;
    bool run_GVNS    = false;

    // Instancias seleccionadas (índices sobre la lista de ficheros)
    std::vector<int> instance_indexes;

    // Parámetros GRASP
    int   grasp_alpha  = 3;
    int   grasp_iters  = 100;
    LSMode grasp_ls_mode = LSMode::ONLY_LS;

    // Parámetros comunes de búsqueda local (para Greedy y GRASP)
    LSMode greedy_ls_mode = LSMode::ONLY_LS;   // se usa en Greedy
    std::vector<bool> use_operators;            // 4 bools: Shift, SwapClientes, SwapFacilities, IncompElim
    std::vector<int>  rvnd_order;               // orden si se usa RVND

     // Búsqueda local compartida
    bool   use_rvnd = false;          // true => RVND, false => VND simple
    bool   use_rl   = false;          // true => VND-RL
    int    rl_reward_type = 1;        // 1=binaria, 2=proporcional
    bool   rl_decay = false;          // decaimiento de epsilon
    double rl_max_delta = 1.0;
    
    // Parámetros GVNS
    bool   gvns_kmax_percent = false;
    double gvns_kmax_value   = 3.0;
    int    gvns_iter      = 100;
    bool   gvns_use_rl    = false;  // si true, la mejora interna es VND-RL
    double gvns_alpha_rl  = 0.1;
    double gvns_epsilon   = 0.2;
    // También se pueden reutilizar use_operators y rvnd_order para la mejora de GVNS
    int    rl_max_sin_mejora = 5;
    int    rl_max_total_iter = 10;

    // Salida
    std::string output_results_file = "data/output/results/default/results.txt";
    std::string output_q_dir = "data/output/q_evolution/default/";
    std::string output_config_log  = "data/output/configs/default/config_log.txt";

    // MOdificacion
    bool run_modified_gvns = true;
};

#endif