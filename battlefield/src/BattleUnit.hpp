/*
 * File: BattleUnit.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleUnit class is the superclass for all autonomous units in the
 *      battle simulation.
 */

#ifndef BATTLEFIELD_BATTLE_UNIT
#define BATTLEFIELD_BATTLE_UNIT

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattleUnit;
    class APC;
    class Humvee;
    class LightTank;
    class HeavyTank;

    // Enumeration of the possible states a BattleUnit can assume
    enum BattleAction {
        Searching,      // Looking for a target
        Attacking,      // Engaged in battle with a target
        Following,      // Following a leader in formation
        Fleeing,        // Attempting to escape an attacker
        Evading,        // Atttempting to dodge an attack
        Uncontrolled,   // At the mercy of other forces (e.g., hit by missile)
        Destroyed,      // Shall we say, "out of the picture"
    };

    // Pointer type definitions
    typedef shared_ptr<BattleUnit> BattleUnitPtr;
};


class Battlefield::BattleUnit : public Inca::World::SolidObject3D {
protected:
    // Constructor, giving model to load
    BattleUnit(const string &model);

public:
    // Ccontrol/display properties
    property_rw(bool, selected, false);             // Has the user selected this unit?
    property_rw(bool, manualControl, false);        // Has the user taken over control?
    property_rw(bool, renderGoal, false);           // Should we draw my goal?
    property_rw(scalar_t, throttle, 0.0);
    property_rw(scalar_t, brake, 0.0);
    property_rw(scalar_t, wheelDeflection, 0.0);

    // Physical properties
    property_rw(scalar_t, mass, 100.0);             // Duh! The mass of the unit
    property_rw(scalar_t, elevationOffset, 0.0);    // Height of center off ground
    property_rw(scalar_t, axleOffset, 0.5);         // How far from CoM is turning axle
    property_rw(scalar_t, maxEngineForce, 10.0);
    property_rw(scalar_t, minTurningRadius, 1.0);
    property_rw(scalar_t, minLinearFriction, 0.1);  // Friction coefficients
    property_rw(scalar_t, minAngularFriction, 0.1); // with no brakes applied
    property_rw(scalar_t, maxLinearFriction, 0.3);  // Friction coefficients
    property_rw(scalar_t, maxAngularFriction, 0.5); // with full braking
    property_rw_ptr(RigidBody, rigidBody, NULL);

    // Combat properties
    property_rw(unsigned int, armor, 100);
    property_rw(unsigned int, ammo, 100);
    property_rw(unsigned int, morale, 100);

    // AI properties
    property_rw(unsigned int, team, 0);
    property_rw(unsigned int, rank, 0);
    property_rw(BattleAction, action, Searching);
    property_rw_ptr(BattleUnit, target, NULL);
    property_rw(Vector, targetOffset, Vector(0.0, 0.0, -1.0));
    property_rw(bool, relativeOffset, false);

    // Goal-reaching (relative) temporary values
    property_rw(Vector, goalDisplacement, Vector(0.0));
    property_rw(Vector, goalVelocity, Vector(0.0));

    // Update my appearance to reflect my state
    void updateTessellation(const Point &view, const Vector &look);

protected:
    // What material index to use for drawing goal targets
    index_t goalMaterialIndex;
};


class Battlefield::APC : public BattleUnit {
public:
    APC();
};


class Battlefield::Humvee : public BattleUnit {
public:
    Humvee();
};


class Battlefield::LightTank : public BattleUnit {
public:
    LightTank();
};


class Battlefield::HeavyTank : public BattleUnit {
public:
    HeavyTank();
};

#endif