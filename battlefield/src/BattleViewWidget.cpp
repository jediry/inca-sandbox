/*
 * File: BattleViewWidget.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      This file implements the BattleViewWidget class.
 */

// Import class definition
#include "BattleViewWidget.hpp"

// Import application definition
#include "BattlefieldApplication.hpp"
using namespace Battlefield;

// How much we move the camera by
const Transform::scalar_t CAMERA_PHI_INCREMENT = Transform::PI / 20.0;
const Transform::scalar_t MIN_CAMERA_PHI = Transform::PI / 6.0;
const Transform::scalar_t MAX_CAMERA_PHI = Transform::PI / 2.0;

// Import OpenGL
#if __MS_WINDOZE__
    // Windows OpenGL seems to need this
#   include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

BattleViewWidget::BattleViewWidget(BattleScenePtr bs, BattleCameraPtr bc)
        : SceneView(ScenePtr(), static_pointer_cast<Camera>(bc)),
          CameraControl(static_pointer_cast<Camera>(bc)),
          frameCapture(false), frameCount(0), selectedUnit(0) {
    setBattleScene(bs);
    battleCamera = bc;
}

void BattleViewWidget::setBattleScene(BattleScenePtr bs) {
    SET_VALUE(battleScene, bs);
    setScene(static_pointer_cast<Scene>(bs));
}

// Rendering functions
void BattleViewWidget::resizeView(unsigned int w, unsigned int h) {
    SceneView::resizeView(w, h);
    width = w;
    height = h;
}

void BattleViewWidget::initializeView() {
    SceneView::initializeView();
}

void BattleViewWidget::renderView() {
    SceneView::renderView();

    if (frameCapture) {
#if 1
        char filename[30];
        sprintf(filename, "%s%03d%s", framePrefix.c_str(), frameCount, frameSuffix.c_str());
        cerr << "Writing image " << filename << endl;
        if (frameCount == 0) {
            ImagePtr img(new ImageRGB(width, height));
            glReadPixels(0, 0, width, height, GL_RGB, GL_BYTE, img->pixels().getContents());
            Inca::IO::storeImage(img, filename);
        }
        frameCount++;
#endif
    }
}

// Event-handler functions
void BattleViewWidget::keyPressed(KeyCode key,
                                  unsigned int x, unsigned int y) {
    switch (key) {
        case KEY_ESCAPE:    application->exit(0, "Exited normally");
        case KEY_P:         togglePaused();                 break;
        case KEY_SPACE:     toggleFullScreen();             break;

        case KEY_TILDE:     toggleGoalMarkers();            break;
        case KEY_ENTER:     toggleManualControl();          break;
        case KEY_TAB:
            if (!flagsSet(SHIFT_KEY))   selectNextUnit();
            else                        selectPreviousUnit();
            break;
        case KEY_UP:        accelerateSelectedUnit();       break;
        case KEY_DOWN:      brakeSelectedUnit();            break;
        case KEY_LEFT:      turnSelectedUnit(1.0);          break;
        case KEY_RIGHT:     turnSelectedUnit(-1.0);         break;

        // Camera controls
        case KEY_1: case KEY_2: case KEY_3: case KEY_4:
            orbitCamera(key - KEY_1);                       break;
        case KEY_6: case KEY_7: case KEY_8: case KEY_9:
            orbitCamera(key - KEY_1 - 1); /* ignore '5' */  break;
        case KEY_MINUS:     elevateCamera(1);               break;
        case KEY_PLUS:      elevateCamera(-1);              break;
    }
}

void BattleViewWidget::togglePaused() {
    Timer &timer = BattlefieldApplication::instance().timer();
    if (timer.isRunning())  timer.stop();
    else                    timer.start();
}

void BattleViewWidget::toggleFullScreen() {
    container()->setFullScreen(! container()->isFullScreen());
}

void BattleViewWidget::toggleFrameCapture() {
    frameCapture = !frameCapture;
}

void BattleViewWidget::setFilenamePattern(const string &pre, const string &suff) {
    framePrefix = pre;
    frameSuffix = suff;
}

void BattleViewWidget::elevateCamera(int clicks) {
    Transform::scalar_t newPhi = battleCamera->targetPhi + clicks * CAMERA_PHI_INCREMENT;

    // Clamp to valid domain
    if (newPhi > MAX_CAMERA_PHI)        newPhi = MAX_CAMERA_PHI;
    else if (newPhi < MIN_CAMERA_PHI)   newPhi = MIN_CAMERA_PHI;

    // If we should still move...
    if (newPhi != battleCamera->targetPhi)
        battleCamera->targetPhi = newPhi;
}

void BattleViewWidget::orbitCamera(int slot) {
    Transform::scalar_t newTheta;
    switch (slot) {
        case 1: newTheta = 0.0;                     break;    
        case 2: newTheta = Transform::PI * 0.25;    break;    
        case 4: newTheta = Transform::PI * 0.50;    break;    
        case 7: newTheta = Transform::PI * 0.75;    break;    
        case 6: newTheta = Transform::PI * 1.00;    break;    
        case 5: newTheta = Transform::PI * 1.25;    break;    
        case 3: newTheta = Transform::PI * 1.50;    break;    
        case 0: newTheta = Transform::PI * 1.75;    break;    
    }

    // If we should still move...
    if (newTheta != battleCamera->targetTheta)
        battleCamera->targetTheta = newTheta;
}

void BattleViewWidget::selectUnit(index_t unit) {
    if (unit < battleScene()->battleUnitCount()) {
        battleScene()->battleUnit(selectedUnit)->selected = false;
        battleScene()->battleUnit(selectedUnit)->manualControl = false;
        battleScene()->battleUnit(selectedUnit)->renderGoal = false;
        selectedUnit = unit;
        battleScene()->battleUnit(selectedUnit)->selected = true;
        battleScene()->battleUnit(selectedUnit)->renderGoal = true;
        cerr << "Selected unit " << selectedUnit << '\n';
    }
    requestRedisplay();
}

void BattleViewWidget::selectPreviousUnit() {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0)
        selectUnit((selectedUnit + count - 1) % count);
}

void BattleViewWidget::selectNextUnit() {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0)
        selectUnit((selectedUnit + 1) % count);
}

void BattleViewWidget::toggleGoalMarkers() {
    for (index_t i = 0; i < battleScene()->battleUnitCount(); i++)
        battleScene()->battleUnit(i)->renderGoal = ! battleScene()->battleUnit(i)->renderGoal;
}

void BattleViewWidget::toggleManualControl() {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0) {
        BattleUnitPtr bu = battleScene()->battleUnit(selectedUnit);
        bu->manualControl = !bu->manualControl;
    }
    requestRedisplay();
}

void BattleViewWidget::accelerateSelectedUnit() {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0) {
        BattleUnitPtr bu = battleScene()->battleUnit(selectedUnit);
        if (bu->manualControl) {
            if (bu->brake != 0.0)
                bu->brake = 0.0;
            else
                bu->throttle = 1.0;
        }
    }
}

void BattleViewWidget::brakeSelectedUnit() {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0) {
        BattleUnitPtr bu = battleScene()->battleUnit(selectedUnit);
        if (bu->manualControl) {
            if (bu->throttle != 0.0)
                bu->throttle = 0.0;
            else
                bu->brake = 1.0;
        }
    }
}

void BattleViewWidget::turnSelectedUnit(scalar_t rate) {
    size_t count = battleScene()->battleUnitCount();
    if (count > 0) {
        BattleUnitPtr bu = battleScene()->battleUnit(selectedUnit);
        if (bu->manualControl) {
            Transform::scalar_t wd = bu->wheelDeflection + rate;
            if (wd > 1.0)          wd = 1.0;
            else if (wd < -1.0)    wd = -1.0;
            bu->wheelDeflection = wd;
        }
    }
}
