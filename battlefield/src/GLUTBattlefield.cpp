/*
 * File: GLUTBattlefield.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      This file integrates all the parts of the simulation into a coherent
 *      application. It contains the main() function and instantiates the
 *      application using the GLUT toolkit.
 */

// Import system configuration and Inca libraries
#include "battlefield-common.h"

// This is part of the Battlefield simulation
namespace Battlefield {
    // Forward declarations
    class GLUTBattlefield;
};

// GUI layout parameters
const string WINDOW_TITLE = "Battlefield";
const size_t WINDOW_WIDTH  = 648;
const size_t WINDOW_HEIGHT = 486;
const bool FULL_SCREEN = false;
const bool FRAME_CAPTURE = true;
const string FRAME_PREFIX = "frame";
const string FRAME_SUFFIX = ".jpg";


// Import the main application class definition
#include "BattlefieldApplication.hpp"
using namespace Battlefield;

// Import GLUT-specific stuff
#include <interface/glut/GLUTFramework.h++>
using namespace Inca::Interface;

// Application class definition
class Battlefield::GLUTBattlefield : public BattlefieldApplication,
                                     public GLUTApplication {
public:
    // Function required by Application
    void constructInterface() {
        window = new GLUTWidgetContainer(WINDOW_TITLE);
        window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        window->setWidget(battleViewWidget.get());
        window->setFullScreen(FULL_SCREEN);
        battleViewWidget->setFilenamePattern(FRAME_PREFIX, FRAME_SUFFIX);
        if (FRAME_CAPTURE)
            battleViewWidget->toggleFrameCapture();
    }

protected:
    GLUTWidgetContainer *window;
};


/*****************************************************************************
 * GLUTBattle main() entry function -- this creates the application and
 * launches it.
 *****************************************************************************/
int main(int argc, char **argv) {
    GLUTBattlefield app;
    app.initialize(argc, argv);
    return app.run();
}
