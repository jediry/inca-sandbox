/*
 * File: BattleCamera.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleCamera class is a perspective camera customized so as to
 *      find its way around the battlefield.
 */

#ifndef BATTLEFIELD_BATTLE_CAMERA
#define BATTLEFIELD_BATTLE_CAMERA

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattleCamera;
    
    // Pointer typedefs
    typedef shared_ptr<BattleCamera> BattleCameraPtr;
};


class Battlefield::BattleCamera : public PerspectiveCamera {
public:
    // Where we are now
    property_rw(Transform::scalar_t, rho, 15.0);
    property_rw(Transform::scalar_t, theta, 0.0);
    property_rw(Transform::scalar_t, phi, Transform::PI / 4.0);
    property_rw(Transform::Point, lookAt, Transform::Point(0.0));

    // Where we're going and where we started from
    property_rw(Transform::scalar_t, targetRho, 15.0);
    property_rw(Transform::scalar_t, targetTheta, 0.0);
    property_rw(Transform::scalar_t, targetPhi, Transform::PI / 4.0);
    property_rw(Transform::Point, targetLookAt, Transform::Point(0.0));

    property_rw(Transform::scalar_t, startRho, 15.0);
    property_rw(Transform::scalar_t, startTheta, 0.0);
    property_rw(Transform::scalar_t, startPhi, Transform::PI / 4.0);
    property_rw(Transform::Point, startLookAt, Transform::Point(0.0));

    // Interpolation controls
    property_rw(bool, inTransit, false);
    property_rw(Transform::scalar_t, t, 0.0);

    // Do the motion calculation
    void update(double time);
    scalar_t cubicBezierInterpolate(scalar_t p0, scalar_t p1,
                                    scalar_t p2, scalar_t p3,
                                    scalar_t t) const;
    Point cubicBezierInterpolate(Point p0, Point p1,
                                 Point p2, Point p3,
                                 scalar_t t) const;
};

#endif
