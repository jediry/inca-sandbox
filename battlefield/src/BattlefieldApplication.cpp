/*
 * File: BattlefieldApplication.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 */

// Import class definition
#include "BattlefieldApplication.hpp"
using namespace Battlefield;

// Camera parameters
const Transform::scalar_t CAMERA_INITIAL_RHO   = 1.0;
const Transform::scalar_t CAMERA_INITIAL_THETA = Transform::PI;
const Transform::scalar_t CAMERA_INITIAL_PHI   = Transform::PI / 1.9;
const Transform::Point    CAMERA_INITIAL_LOOK_AT(0.0, 0.0, -10.0);

const Transform::scalar_t CAMERA_TARGET_RHO   = 12.0;
const Transform::scalar_t CAMERA_TARGET_THETA = Transform::PI / 5.0;
const Transform::scalar_t CAMERA_TARGET_PHI   = Transform::PI / 4.0;
const Transform::Point    CAMERA_TARGET_LOOK_AT(0.0, 0.0, 0.0);

// Timer parameters
const double START_TIME = 0.0;
const double END_TIME   = 100000.0;
const double TIME_SCALE = 1.0;
const double TIME_STEP  = 0.05;
const bool   ALLOW_SKIP = false;

// Battle setup parameters
const Transform::Vector ROW_OFFSET(0.5, 0.0, 0.0);
const Transform::Vector ECHELON_OFFSET(0.2, 0.0, 0.4);
const Transform::Vector DIAMOND_OFFSET(0.4, 0.0, 0.5);


// Singleton pointer instantiation
BattlefieldApplication * BattlefieldApplication::baInstance;


void BattlefieldApplication::setup(int &argc,  char **argv) {
    initializeTimer();
    initializeCamera();
    initializeBattleScene();

    battleViewWidget = BattleViewWidgetPtr(new BattleViewWidget(battleScene,
                                                                battleCamera));

    // Make sure the simulation/camera start with valid values
    update(0.0);
}

void BattlefieldApplication::initializeCamera() {
    battleCamera = BattleCameraPtr(new BattleCamera());
    battleCamera->startRho    = CAMERA_INITIAL_RHO;
    battleCamera->rho         = CAMERA_INITIAL_RHO;
    battleCamera->startTheta  = CAMERA_INITIAL_THETA;
    battleCamera->theta       = CAMERA_INITIAL_THETA;
    battleCamera->startPhi    = CAMERA_INITIAL_PHI;
    battleCamera->phi         = CAMERA_INITIAL_PHI;
    battleCamera->startLookAt = CAMERA_INITIAL_LOOK_AT;
    battleCamera->lookAt      = CAMERA_INITIAL_LOOK_AT;
}

void BattlefieldApplication::initializeTimer() {
    timer().setTimeScale(TIME_SCALE);
    timer().setMinimumTime(START_TIME);
    timer().setMaximumTime(END_TIME);
    timer().setPulseInterval(TIME_STEP);
    timer().setMaySkipPulses(ALLOW_SKIP);
    timer().addTimerListener(this);
    timer().start();
}

void BattlefieldApplication::initializeBattleScene() {
    battleScene = BattleScenePtr(new BattleScene());
    BattleUnitPtr lt = battleScene->addBattleUnit("light tank");
    BattleUnitPtr ht = battleScene->addBattleUnit("heavy tank");
    BattleUnitPtr hv = battleScene->addBattleUnit("humvee");
    BattleUnitPtr pc = battleScene->addBattleUnit("APC");
    
    pc->transform->translate(Transform::Vector(0.0, 0.0, 3.0));
    hv->transform->translate(Transform::Vector(0.0, 0.0, -3.0));
    hv->transform->rotateY(-Transform::PI);
    lt->transform->translate(Transform::Vector(3.0, 0.0, 0.0));
    lt->transform->rotateY(Transform::PI / 2.0);
    ht->transform->translate(Transform::Vector(-3.0, 0.0, 0.0));
    ht->transform->rotateY(-Transform::PI / 2.0);

    battleScene->createRow(hv, "light tank", ROW_OFFSET, 4);
    battleScene->createRow(ht, "humvee", ECHELON_OFFSET, 4);
    battleScene->createDiamond(lt, "APC", DIAMOND_OFFSET);
    battleScene->createTriangle(pc, "heavy tank", DIAMOND_OFFSET);
}

void BattlefieldApplication::update(double time) {
    // Advance the simulation
    battleScene->update(time);

    // Move the camera (if necessary)
    battleCamera->update(time);

    // Redisplay the view
    battleViewWidget->requestRedisplay();
}