#include "./swap_clients.h"
#include <limits>

bool SwapClients::improve(Solution& solution, const Instance& inst) {
    bool any_improved = false;

    bool found_move = true;
    while (found_move) {
        found_move = false;

        double bestDelta = -EPS;
        int bestI1 = -1, bestI2 = -1, bestJ1 = -1, bestJ2 = -1;

        // Solo considerar clientes servidos íntegramente por una instalación
        for (int i1 = 0; i1 < inst.stores; ++i1) {
            if ((int)solution.facilities_of[i1].size() != 1) continue;
            const int j1 = solution.facilities_of[i1][0];

            for (int i2 = i1 + 1; i2 < inst.stores; ++i2) {
                if ((int)solution.facilities_of[i2].size() != 1) continue;
                const int j2 = solution.facilities_of[i2][0];
                if (j1 == j2) continue; // misma instalación, sin intercambio útil

                const double d1 = inst.demand[i1];
                const double d2 = inst.demand[i2];

                // ── Verificación de capacidad ────────────────────────────
                // j1 pierde d1 y gana d2 → necesita residual_cap[j1] + d1 >= d2
                if (solution.residual_cap[j1] + d1 < d2 - EPS) continue;
                // j2 pierde d2 y gana d1 → necesita residual_cap[j2] + d2 >= d1
                if (solution.residual_cap[j2] + d2 < d1 - EPS) continue;

                // ── Verificación de incompatibilidades ───────────────────
                // i1 pasa a j2: comprobar incomp_count[i1][j2]
                //   Si i1 e i2 son incompatibles, i2 está saliendo de j2 → restar 1
                int incompI1inJ2 = solution.incomp_count[i1][j2];
                if (inst.is_incompat[i1][i2]) --incompI1inJ2;
                if (incompI1inJ2 > 0) continue;

                // i2 pasa a j1: comprobar incomp_count[i2][j1]
                //   Si i1 e i2 son incompatibles, i1 está saliendo de j1 → restar 1
                int incompI2inJ1 = solution.incomp_count[i2][j1];
                if (inst.is_incompat[i1][i2]) --incompI2inJ1;
                if (incompI2inJ1 > 0) continue;

                // ── Variación de coste ───────────────────────────────────
                const double delta =
                    (inst.supply_cost[i1][j2] - inst.supply_cost[i1][j1]) * d1 +
                    (inst.supply_cost[i2][j1] - inst.supply_cost[i2][j2]) * d2;

                if (delta < bestDelta) {
                    bestDelta = delta;
                    bestI1 = i1; bestI2 = i2;
                    bestJ1 = j1; bestJ2 = j2;
                }
            }
        }

        // Aplicar el mejor intercambio encontrado
        if (bestI1 >= 0) {
            const double d1 = inst.demand[bestI1];
            const double d2 = inst.demand[bestI2];
            solution.removeAssignment(bestI1, bestJ1, d1, inst);
            solution.removeAssignment(bestI2, bestJ2, d2, inst);
            solution.assign(bestI1, bestJ2, d1, inst);
            solution.assign(bestI2, bestJ1, d2, inst);
            found_move   = true;
            any_improved = true;
        }
    }

    return any_improved;
}