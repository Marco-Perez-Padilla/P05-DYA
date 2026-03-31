#pragma once
#include "../model/instance.h"
#include "../model/solution.h"

/**
 * @brief Interfaz de estrategia para algoritmos constructivos.
 *
 * Toda clase que construya una solución inicial debe heredar de esta
 * y sobrescribir build().
 */
class Constructive {
public:
    virtual ~Constructive() = default;
    virtual Solution build(const Instance& inst) = 0;
};