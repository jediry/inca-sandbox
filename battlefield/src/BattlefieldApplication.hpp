/*
 * File: BattlefieldApplication.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 */

#ifndef BATTLEFIELD_APPLICATION
#define BATTLEFIELD_APPLICATION

// Import system configuration and Inca libraries
#include "battlefield-common.h"


// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattlefieldApplication;
};


// Import the Battlefield class definitions
#include "BattleUnit.hpp"
#include "BattleScene.hpp"
#include "BattleCamera.hpp"
#include "BattleViewWidget.hpp"


// Application class definition
class Battlefield::BattlefieldApplication : virtual public Application,
                                            public TimerListener {
public:
    // Constructor (defers most init for Application-inherited functions)
    BattlefieldApplication() { baInstance = this; }

    // Singleton access function
    static BattlefieldApplication & instance() { return *baInstance; }

protected:
    static BattlefieldApplication * baInstance;


public:
    // Read command-line arguments and configure ourselves
    void setup(int &argc, char **argv);

    // Subordinate initialization functions
    void initializeCamera();
    void initializeTimer();
    void initializeBattleScene();

    // System-update function (should be called repeatedly from timer)
    void timerPulsed(double time) { update(time); }
    void update(double time);

protected:
    BattleViewWidgetPtr battleViewWidget;
    BattleScenePtr battleScene;
    BattleCameraPtr battleCamera;
};

#endif
