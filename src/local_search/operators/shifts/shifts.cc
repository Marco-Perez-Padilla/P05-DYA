#include "./shifts.h"
#include <limits>
#include <algorithm>

bool Shifts::improve(Solution& solution, const Instance& inst) {
    bool any_improved = false;

    // Repetir hasta que no haya movimientos de mejora
    bool found_move = true;
    while (found_move) {
        found_move = false;

        double bestDelta = -EPS; // solo aceptamos mejoras estrictas
        int    bestI = -1, bestJ1 = -1, bestJ2 = -1;
        double bestQ = 0.0;

        // Recorrer todos los clientes
        for (int i = 0; i < inst.stores; ++i) {
            // j1: instalaciones que sirven actualmente a i
            for (int j1 : solution.facilities_of[i]) {
                const double amountAtJ1 = solution.demand_fraction[i][j1] * inst.demand[i];
                if (amountAtJ1 < EPS) continue;

                // j2: otras instalaciones abiertas
                for (int j2 = 0; j2 < inst.warehouses; ++j2) {
                    if (!solution.open[j2] || j2 == j1) continue;

                    // Solo merece explorar si j2 es más barato que j1 para i
                    if (inst.supply_cost[i][j2] >= inst.supply_cost[i][j1]) continue;

                    // Verificar compatibilidad en j2:
                    //   - Si i ya está en j2 (partial_attend[i][j2]=true): compatible por definición
                    //   - Si no: ningún incompatible de i debe estar en j2
                    if (!solution.partial_attend[i][j2] && solution.incomp_count[i][j2] > 0) continue;

                    // Verificar capacidad residual en j2
                    if (solution.residual_cap[j2] < EPS) continue;

                    // Cantidad máxima transferible
                    const double q = std::min(amountAtJ1, solution.residual_cap[j2]);
                    if (q < EPS) continue;

                    // Variación de coste de transporte (negativa = mejora)
                    const double delta = (inst.supply_cost[i][j2] - inst.supply_cost[i][j1]) * q;

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestI = i; bestJ1 = j1; bestJ2 = j2;
                        bestQ = q;
                    }
                }
            }
        }

        // Aplicar el mejor movimiento encontrado
        if (bestI >= 0) {
            solution.removeAssignment(bestI, bestJ1, bestQ, inst);
            solution.assign(bestI, bestJ2, bestQ, inst);
            found_move    = true;
            any_improved  = true;
        }
    }

    return any_improved;
}