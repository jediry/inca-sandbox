/*
 * File: BattleCamera.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleCamera class is a perspective camera customized so as to
 *      find its way around the battlefield.
 */

// Import class definition
#include "BattleCamera.hpp"
using namespace Battlefield;

void BattleCamera::update(double time) {
    bool changedPosition = false,
         changedLookAt   = false;

    // Find disparity between current and target values
    scalar_t dRho = targetRho - rho;
    scalar_t dTheta = targetTheta - theta;
    scalar_t dThetaAlt = targetTheta - 2.0 * PI - theta;
    scalar_t dPhi = targetPhi - phi;
    Vector dLookAt = Point(targetLookAt) - Point(lookAt);

    // Fix dTheta to account for the discontinuity at 2pi == 0.0
    if (abs(dTheta) > abs(dThetaAlt))
        dTheta = dThetaAlt;


    // Interpolate rho
    if (dRho != 0.0) {
        rho = scalar_t(targetRho);
        changedPosition = true;
    } else if (targetRho != startRho) {         // We got there!
        startRho = scalar_t(targetRho);
    }

    // Interpolate theta
    if (dTheta != 0.0) {
        theta = scalar_t(targetTheta);
        changedPosition = true;
    } else if (targetTheta != startTheta) {     // We got there!
        startTheta = scalar_t(targetTheta);
    }

    // Interpolate phi
    if (dPhi != 0.0) {
        phi = scalar_t(targetPhi);
        changedPosition = true;
    } else if (targetPhi != startPhi) {         // We got there!
        startPhi = scalar_t(targetPhi);
    }

    // Interpolate lookAt
    if (dLookAt != Vector(0.0)) {
        lookAt = Point(targetLookAt);
        changedLookAt = true;
    } else if (Point(targetLookAt) != Point(startLookAt)) {   // We got there!
        startLookAt = Point(targetLookAt);
    }

    // If anything's changed, we should tell the Transform
    if (changedPosition) {
        scalar_t sin_phi = sin(phi);
        scalar_t x = rho * cos(theta) * sin_phi;
        scalar_t z = -(rho * sin(theta) * sin_phi);
        scalar_t y = rho * cos(phi);
        transform->setLocationPoint(new Point(x, y, z));
    }

    if (changedLookAt || changedPosition) {
        transform->lookAt(lookAt, Vector(0.0, 1.0, 0.0));
    }
}

Transform::scalar_t BattleCamera::cubicBezierInterpolate(scalar_t p0, scalar_t p1,
                                                         scalar_t p2, scalar_t p3,
                                                         scalar_t t) const {
    scalar_t t_1 = 1.0 - t;
    return p0 * t_1 * t_1 * t_1
         + p1 * t_1 * t_1 * t
         + p2 * t_1 * t   * t
         + p3 * t   * t   * t;
}

Transform::Point BattleCamera::cubicBezierInterpolate(Transform::Point p0, Transform::Point p1,
                                                      Transform::Point p2, Transform::Point p3,
                                                      scalar_t t) const {
    return Point(cubicBezierInterpolate(p0[0], p1[0], p2[0], p3[0], t),
                 cubicBezierInterpolate(p0[1], p1[1], p2[1], p3[1], t),
                 cubicBezierInterpolate(p0[2], p1[2], p2[2], p3[2], t));
}