/*
 * File: Friction.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleUnitControl class is
 */

#ifndef BATTLEFIELD_SIMPLE_FRICTION
#define BATTLEFIELD_SIMPLE_FRICTION

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class SimpleFriction;
};


class Battlefield::SimpleFriction
             : public RigidBodySystem::SecondDerivativeOperator {
public:
    // Constructor
    SimpleFriction(scalar_arg_t kl, scalar_arg_t ka)
        : kLinear(kl), kAngular(ka) { }

    // Dynamics function
    void modifySecondDerivative(SystemState &delta, SystemCalculation &calc,
                                SystemState &previous, const ObjectPtrList &objects) {
        for (index_t i = 0; i < objects.size(); i++) {
        }
    }

protected:
    scalar_t kLinear, kAngular;
};

#endif
