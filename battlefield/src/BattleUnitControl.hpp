/*
 * File: BattleUnitControl.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleUnitControl class is
 */

#ifndef BATTLEFIELD_BATTLE_UNIT_CONTROL
#define BATTLEFIELD_BATTLE_UNIT_CONTROL

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattleUnitControl;
};


// Import other battle type definitions
#include "BattleUnit.hpp"


class Battlefield::BattleUnitControl
             : public RigidBodySystem::ThirdDerivativeOperator,
               public RigidBodySystem::SecondDerivativeOperator {
public:
    // Constructor
    BattleUnitControl(BattleUnitPtr bu, ObjectPtr rb)
        : battleUnit(bu), rigidBody(rb), myIndex(0) { }

    // Artificial intelligence functions
    void calculateGoal();
    void tryToReachGoal();
    void modifyThirdDerivative(SystemState &delta,
                               SystemCalculation &calc,
                         const SystemState &prev,
                         const ObjectPtrList &objects);

    // Dynamics function
    void modifySecondDerivative(SystemState &delta,
                                SystemCalculation &calc,
                          const SystemState &prev,
                          const ObjectPtrList &objects);


protected:
    BattleUnitPtr battleUnit;
    ObjectPtr rigidBody;
    index_t myIndex;
};

#endif
