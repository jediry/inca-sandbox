/*
 * File: BattleUnitControl.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleUnitControl class is
 */

// Import class definitions
#include "BattleUnitControl.hpp"
using namespace Battlefield;


const Transform::Vector Ypos(0.0, 1.0, 0.0);
const Transform::Vector Zneg(0.0, 0.0, -1.0);

// Determine our goal displacement and velocity within our coordinate frame
void BattleUnitControl::calculateGoal() {
    BattleUnitPtr target = battleUnit->target;
    const Point &tLocation = *target->transform->locationPoint();
    const Point &mLocation = *battleUnit->transform->locationPoint();
    Vector tVelocity = target->rigidBody->P / target->mass;
    Vector mVelocity = battleUnit->rigidBody->P / battleUnit->mass;
    const Quaternion &tRotation = target->transform->rotation();
    const Quaternion &mRotation = battleUnit->transform->rotation();

    const Vector &offset = battleUnit->targetOffset;
    Point worldLocation;
    if (battleUnit->relativeOffset) {
        worldLocation = tLocation + tRotation.rotate(offset);
    } else {
        worldLocation = tLocation + offset;
    }

    Vector relativeDistance = worldLocation - *battleUnit->transform->locationPoint();
    Vector relativeVelocity = tVelocity - mVelocity;

//    if (battleUnit->selected)
//        cerr << "Target is " << relativeDistance << " away, with relative V " << relativeVelocity << endl;

    battleUnit->goalDisplacement = relativeDistance;
    battleUnit->goalVelocity = relativeVelocity;
}


// Set our controls (throttle, brake, wheels) in an attempt to meet the goal
void BattleUnitControl::tryToReachGoal() {
    // Look up values
    Vector myVelocity = battleUnit->rigidBody->P / battleUnit->mass;
    const Vector &relDisplacement = battleUnit->goalDisplacement;
    const Vector &relVelocity = battleUnit->goalVelocity;

    // Calculate our metrics
    scalar_t distance = magnitude(relDisplacement);
    scalar_t relativeSpeed = magnitude(relVelocity);
    scalar_t signedSpeed = dot(relVelocity, myVelocity);
    scalar_t angleD = angle(Zneg, -relDisplacement, Ypos);
    scalar_t angleV = angle(Zneg, relVelocity, Ypos);

    // Pick velocity based on distance and relative velocity
    if (relativeSpeed > 2.0)
        battleUnit->throttle = battleUnit->throttle + 0.3;
    else if (relativeSpeed < 0.5)
        battleUnit->throttle = battleUnit->throttle - 0.2;

    if (distance > 1.0)
        battleUnit->throttle = battleUnit->throttle + 0.3;
    else if (distance < 0.5)
        battleUnit->throttle = battleUnit->throttle - 0.1;


    // Pick turn angle based on angle to target
    battleUnit->wheelDeflection = 2.0 * angleD / Transform::PI;

    if (battleUnit->selected)
        cerr << "AngleD: " << angleD << "\n";

    if (battleUnit->throttle > 1.0)         battleUnit->throttle = 1.0;
    else if (battleUnit->throttle < 0.0)    battleUnit->throttle = 0.0;

    if (battleUnit->wheelDeflection > 1.0)          battleUnit->wheelDeflection = 1.0;
    else if (battleUnit->wheelDeflection < -1.0)    battleUnit->wheelDeflection = -1.0;

    if (battleUnit->brake > 1.0)            battleUnit->brake = 1.0;
    else if (battleUnit->brake < 0.0)       battleUnit->brake = 0.0;
}


// Artificial intelligence function
void BattleUnitControl::modifyThirdDerivative(SystemState &delta,
                                              SystemCalculation &calc,
                                        const SystemState &prev,
                                        const ObjectPtrList &objects) {
    // If our "index-of-me" is wrong, go hunt it down
    if (objects[myIndex] != rigidBody) {
        cerr << "Hunting down index of " << rigidBody << "...";
        myIndex = objects.index_of(rigidBody);
        cerr << myIndex << '\n';
    }

    if (battleUnit->manualControl) {
        // Don't step on the user's toes!
        // Only think if we're not being thought for...

    } else if (battleUnit->target != NULL) {
        // First, we need to know where we're supposed to go
        calculateGoal();
#if 1
        // Modify goal to avoid others
        Vector newTarget(0.0);
        index_t count = 0;
        for (index_t i = 0; i < objects.size(); i++) {
            if (i != myIndex) {
                Vector diff = prev[myIndex].x - prev[i].x;
                if (dot(diff, diff) < 0.5 * 0.5) {
                    count++;
                    newTarget += Vector(prev[i].x + 0.75 * diff);
                }
            }
        }
        if (dot(prev[myIndex].x, prev[myIndex].x) > 3 * 3) {    // Head back
            newTarget += Vector(0.0, 0.0, 0.0);
            count++;
        }
        if (count > 0) {        // Must take evasive action...
            // Convert to me-space
            newTarget = prev[myIndex].q.unrotate(newTarget - prev[myIndex].x)
                      + battleUnit->goalDisplacement;
            count++;
            battleUnit->goalDisplacement = newTarget / count;
        }
#endif
        // Next, we must decide how to reach that
        tryToReachGoal();

    } else {
        // Well...nothing to do but wander aimlessly
        battleUnit->throttle = 0.3;
        if (typeid(*battleUnit) == typeid(Humvee) || typeid(*battleUnit) == typeid(LightTank))
            battleUnit->wheelDeflection = 0.7;
        else
            battleUnit->wheelDeflection = -0.7;
        if (dot(prev[myIndex].x, prev[myIndex].x) > 15) {
            battleUnit->goalDisplacement = Vector(0.0);
            battleUnit->target = battleUnit;
        }
    }
}


// Dynamics function
void BattleUnitControl::modifySecondDerivative(SystemState &delta,
                                               SystemCalculation &calc,
                                         const SystemState &prev,
                                         const ObjectPtrList &objects) {
    // We can safely assume that the index we calculated above is valid

    // Linear velocity within the ground plane
    Vector vTangent = calc[myIndex].v;
    vTangent[1] = 0.0;

    // Angular velocity w/r to the ground plane normal
    scalar_t wTangent = dot(calc[myIndex].w, Ypos);

    // Engine-powered acceleration
    scalar_t throttle = battleUnit->throttle;
    if (throttle != 0.0) {
        scalar_t engine = throttle * battleUnit->maxEngineForce;
        calc[myIndex].F += engine * battleUnit->transform->front();
    }

    // Turning force
    scalar_t wheelDeflection = battleUnit->wheelDeflection;
    if (wheelDeflection != 0.0) {
        // Find force vector
        scalar_t turnF = wheelDeflection * dot(vTangent, vTangent)
                        * battleUnit->mass / battleUnit->minTurningRadius;
        Vector force = turnF * battleUnit->transform->left();

        // Find point of application
        Vector apply = battleUnit->transform->front() * battleUnit->axleOffset;
//        calc[myIndex].F += force;
        calc[myIndex].T += (apply % force);
#if 0
        if (battleUnit->manualControl) {
            cerr << "wheel: " << wheelDeflection << "   mass " << battleUnit->mass << "   v " << vTangent << '\n'
                 << "  v^2: " << dot(vTangent, vTangent) << "   radius " << battleUnit->minTurningRadius << endl;
            cerr << "Applying force " << turnF << " at offset " << battleUnit->axleOffset << endl;
            cerr << "Torque is " << calc[myIndex].T << endl;
        }
#endif
    }

    // Friction acts only if pressed against the ground
    if (calc[myIndex].F[1] < 0.0) {
        // Opposing linear friction force
        scalar_t min = battleUnit->minLinearFriction;
        scalar_t max = battleUnit->maxLinearFriction;
        scalar_t k = min + (max - min) * battleUnit->brake;
        calc[myIndex].F += vTangent * (k * calc[myIndex].F[1]);

        // Opposing angular friction torque
        min = battleUnit->minAngularFriction;
        max = battleUnit->maxAngularFriction;
        k = min + (max - min) * battleUnit->brake;
        calc[myIndex].T += (wTangent * k * calc[myIndex].F[1]) * Ypos;

        // Zero vertical forces/torques
        calc[myIndex].F[1] = 0.0;
        calc[myIndex].T[0] = 0.0;
        calc[myIndex].T[2] = 0.0;
    }
}
