#include "./swap_facilities.h"
#include <limits>
#include <tuple>
#include <vector>

bool SwapFacilities::improve(Solution& sol, const Instance& inst) {
    bool anyImproved = false;

    bool foundMove = true;
    while (foundMove) {
        foundMove = false;

        // Iterar sobre instalaciones abiertas candidatas a cerrar
        for (int jclose = 0; jclose < inst.m && !foundMove; ++jclose) {
            if (!sol.open[jclose]) continue;
            if (sol.clientsOf[jclose].empty()) {
                // Instalación abierta sin clientes: cerrar directamente
                const double delta = -inst.fixedCost[jclose];
                if (delta < -EPS) {
                    sol.closeFacility(jclose, inst);
                    foundMove   = true;
                    anyImproved = true;
                }
                continue;
            }

            // Copia de la lista de clientes (evitar modificar durante iteración)
            const std::vector<int> clients = sol.clientsOf[jclose];

            // Probar: jnew = -1 (cierre puro) y jnew >= 0 (intercambio)
            for (int jnew = -1; jnew < inst.m && !foundMove; ++jnew) {
                if (jnew >= 0 && sol.open[jnew]) continue; // solo cerradas
                if (jnew == jclose) continue;

                // ── Simulación ───────────────────────────────────────────
                // Capacidad residual simulada (solo modificamos localmente)
                std::vector<double> simResidual(inst.m);
                for (int j = 0; j < inst.m; ++j)
                    simResidual[j] = sol.residualCap[j];
                if (jnew >= 0)
                    simResidual[jnew] = inst.capacity[jnew]; // jnew se abre

                double transportDelta = 0.0;
                bool   feasible       = true;

                // Plan de reasignación: (cliente, destino, cantidad)
                std::vector<std::tuple<int,int,double>> plan;
                plan.reserve(clients.size());

                for (int i : clients) {
                    const double q = sol.x[i][jclose] * inst.demand[i];

                    // Buscar la instalación destino más barata compatible y con capacidad
                    int    bestJ    = -1;
                    double bestCost = std::numeric_limits<double>::max();

                    // Priorizar jnew (siempre compatible: aún no tiene clientes)
                    if (jnew >= 0 && simResidual[jnew] >= q - EPS) {
                        bestJ    = jnew;
                        bestCost = inst.supplyCost[i][jnew];
                    }

                    // Buscar entre las demás instalaciones abiertas
                    for (int j = 0; j < inst.m; ++j) {
                        if (j == jclose || j == jnew) continue;
                        if (!sol.open[j])              continue;
                        // Compatibilidad con clientes existentes en j
                        // (los clientes de jclose son mutuamente compatibles,
                        //  por lo que agregar varios de ellos al mismo j no
                        //  genera incompatibilidades entre sí)
                        if (!sol.w[i][j] && sol.incompCount[i][j] > 0) continue;
                        if (simResidual[j] < q - EPS) continue;

                        if (inst.supplyCost[i][j] < bestCost) {
                            bestCost = inst.supplyCost[i][j];
                            bestJ    = j;
                        }
                    }

                    if (bestJ < 0) { feasible = false; break; }

                    simResidual[bestJ] -= q;
                    transportDelta     += (inst.supplyCost[i][bestJ]
                                         - inst.supplyCost[i][jclose]) * q;
                    plan.emplace_back(i, bestJ, q);
                }

                if (!feasible) continue;

                const double fixedDelta = (jnew >= 0 ? inst.fixedCost[jnew] : 0.0)
                                        - inst.fixedCost[jclose];
                const double totalDelta = fixedDelta + transportDelta;

                if (totalDelta < -EPS) {
                    // ── Aplicar el movimiento ────────────────────────────
                    // 1. Eliminar todas las asignaciones de jclose
                    for (auto& [i, dest, q] : plan)
                        sol.removeAssignment(i, jclose, q, inst);
                    sol.closeFacility(jclose, inst);

                    // 2. Abrir jnew (si procede)
                    if (jnew >= 0) sol.openFacility(jnew, inst);

                    // 3. Crear nuevas asignaciones
                    for (auto& [i, dest, q] : plan)
                        sol.assign(i, dest, q, inst);

                    foundMove   = true;
                    anyImproved = true;
                }
            }
        }
    }

    return anyImproved;
}