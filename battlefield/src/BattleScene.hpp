/*
 * File: BattleField.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleField class is subclasses Inca World::Scene and describes
 *      the relationships between all of the objects in the battle.
 */

#ifndef BATTLEFIELD_BATTLE_SCENE
#define BATTLEFIELD_BATTLE_SCENE

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattleScene;

    // Pointer type definitions
    typedef shared_ptr<BattleScene> BattleScenePtr;
};

// Import other battle type definitions
#include "BattleUnit.hpp"


class Battlefield::BattleScene : public Scene {
public:
    // Constructor
    BattleScene();

    // List of battle units
    PTR_PROPERTY_LIST_ADD(battleUnit, BattleUnit, addBattleUnit);
    BattleUnitPtr addBattleUnit(const string &type);

    // Team/formation construction functions
    void createTeam();
    void createRow(BattleUnitPtr leader,
                   const string &unitType,
                   const Vector &offset,
                   size_t number);
    void createDiamond(BattleUnitPtr leader,
                       const string &unitType,
                       const Vector &offset);
    void createTriangle(BattleUnitPtr leader,
                        const string &unitType,
                        const Vector &offset);

    // Simulation update function
    void update(double time);

protected:
    RigidBodySystemPtr system;
    SolidObject3DPtr groundPlane;
    LightPtr sunLight;
};

#endif
