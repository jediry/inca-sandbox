/*
 * File: BattleViewWidget.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The BattleViewWidget is a subclass of the Inca SceneView widget,
 *      specialized for viewing the battle simulation.
 */

#ifndef BATTLEFIELD_BATTLE_VIEW_WIDGET
#define BATTLEFIELD_BATTLE_VIEW_WIDGET

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class BattleViewWidget;
    
    // Pointer declarations
    typedef shared_ptr<BattleViewWidget> BattleViewWidgetPtr;
};


// Import other battle type definitions
#include "BattleScene.hpp"
#include "BattleCamera.hpp"


class Battlefield::BattleViewWidget : public Widget,
                                      public SceneView,
                                      public CameraControl {
public:
    // Constructor
    BattleViewWidget(BattleScenePtr bf, BattleCameraPtr c);

    // Property definitions
    PTR_PROPERTY_SET(battleScene, BattleScene, , setBattleScene);

    // Rendering functions
    void resizeView(unsigned int w, unsigned int h);
    void initializeView();
    void renderView();

    // Event-handler functions
    void keyPressed(KeyCode, unsigned int x, unsigned int y);

    // Application-level controls
    void togglePaused();
    void toggleFullScreen();
    void toggleFrameCapture();
    void setFilenamePattern(const string &pre, const string &suff);

    // View-related controls
    void elevateCamera(int clicks);
    void orbitCamera(int slots);
    void lookAtPoint(index_t p);

    // Manual unit-control
    void selectUnit(index_t unit);
    void selectPreviousUnit();
    void selectNextUnit();
    void toggleGoalMarkers();
    void toggleManualControl();
    void brakeSelectedUnit();
    void accelerateSelectedUnit();
    void turnSelectedUnit(scalar_t deflection);

protected:
    BattleCameraPtr battleCamera;
    unsigned int width, height;
    bool frameCapture;
    index_t frameCount;
    index_t selectedUnit;
    string framePrefix, frameSuffix;
};

#endif
