/*
 * File: GroundConstraint.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *
 */

#ifndef BATTLEFIELD_GROUND_CONSTRAINT
#define BATTLEFIELD_GROUND_CONSTRAINT

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class GroundConstraint;
};


// Import other battle type definitions
#include "BattleUnit.hpp"


class Battlefield::GroundConstraint
             : public RigidBodySystem::ZerothDerivativeOperator {
public:
    // Constructor
    GroundConstraint(scalar_arg_t ge) : groundElevation(ge) { }

    // Dynamics function
    void modifyZerothDerivative(SystemState &delta,
                                SystemCalculation &calc,
                          const SystemState &prev,
                          const ObjectPtrList &objects) {
        for (index_t i = 0; i < objects.size(); i++) {
            BattleUnitPtr bu = static_pointer_cast<BattleUnit>(objects[i]->worldObject);
            scalar_t projectedElev = prev[i].x[1] + delta[i].x[1];
            scalar_t groundElev = bu->elevationOffset + groundElevation;

            // Force non-penetration
            if (projectedElev < groundElev)
                delta[i].x[1] = groundElev - prev[i].x[1];
        }
    }

protected:
    scalar_t groundElevation;
};

#endif
