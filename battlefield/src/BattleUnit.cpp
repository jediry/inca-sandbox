/*
 * File: BattleUnit.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      This file implements the BattleUnit classes defined in BattleUnit.hpp.
 */
 
// Import class definitions
#include "BattleUnit.hpp"
using namespace Battlefield;

// Import Inca stuff
#include <io/IOUtilities.h++>
using namespace Inca::IO;
using namespace Inca::World;


// Shortcut for constructing filenames
#define OBJ(NAME) string("data\\" #NAME)


// Appearance for units in the simulation
const Material::Color UNSELECTED_UNIT_EMISSIVITY(0.0f, 0.0f, 0.0f);
const Material::Color SELECTED_UNIT_EMISSIVITY(0.2f, 0.2f, 0.2f);
const Material::Color MANUAL_UNIT_EMISSIVITY(0.5f, 0.5f, 0.5f);

// Appearance for goal marker
const Material::Color GOAL_COLOR(1.0f, 0.0f, 0.0f);
const Transform::scalar_t GOAL_RADIUS = 1.0;
const Transform::scalar_t GOAL_RAD_2_2 = GOAL_RADIUS * Transform::sqrt(2.0) / 2.0;
const Transform::scalar_t GOAL_MAJOR_TICK_2 = 0.75;
const Transform::scalar_t GOAL_MINOR_TICK_2 = 0.25;
const Transform::scalar_t GOAL_MINOR_TICK_2_4 = GOAL_MINOR_TICK_2 * Transform::sqrt(2.0) / 2.0;
const Transform::scalar_t GOAL_ELEVATION = 0.01;
const Transform::scalar_t GOAL_SCALE = 0.3;
const Transform::Point GOAL_VERTICES[] = {
    Transform::Point(GOAL_RADIUS, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(GOAL_RAD_2_2, GOAL_ELEVATION, -GOAL_RAD_2_2) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, -GOAL_RADIUS) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2, GOAL_ELEVATION, -GOAL_RAD_2_2) * GOAL_SCALE,
    Transform::Point(-GOAL_RADIUS, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2, GOAL_ELEVATION, GOAL_RAD_2_2) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, GOAL_RADIUS) * GOAL_SCALE,
    Transform::Point(GOAL_RAD_2_2, GOAL_ELEVATION, GOAL_RAD_2_2) * GOAL_SCALE,

    Transform::Point(GOAL_RADIUS + GOAL_MAJOR_TICK_2, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(GOAL_RADIUS - GOAL_MAJOR_TICK_2, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, -GOAL_RADIUS - GOAL_MAJOR_TICK_2) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, -GOAL_RADIUS + GOAL_MAJOR_TICK_2) * GOAL_SCALE,
    Transform::Point(-GOAL_RADIUS - GOAL_MAJOR_TICK_2, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(-GOAL_RADIUS + GOAL_MAJOR_TICK_2, GOAL_ELEVATION, 0.0) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, GOAL_RADIUS + GOAL_MAJOR_TICK_2) * GOAL_SCALE,
    Transform::Point(0.0, GOAL_ELEVATION, GOAL_RADIUS - GOAL_MAJOR_TICK_2) * GOAL_SCALE,

    Transform::Point(GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     -GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     -GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     -GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     -GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(-GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     GOAL_RAD_2_2 + GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
    Transform::Point(GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4, GOAL_ELEVATION,
                     GOAL_RAD_2_2 - GOAL_MINOR_TICK_2_4) * GOAL_SCALE,
};
const index_t GOAL_VERTEX_COUNT = 24;


// Superclass constructor
BattleUnit::BattleUnit(const string &model) {
    // Load the model from the file
    SolidObject3DPtr obj = static_pointer_cast<SolidObject3D>(loadModel(model));
    addApproximation(obj->approximation(0));
    for (index_t i = 0; i < obj->materialCount(); i++)
        addMaterial(obj->material(i));
}

// Change our appearance to reflect our current state
void BattleUnit::updateTessellation(const Point &view, const Vector &look) {
    // Do the normal tessellation update
    SolidObject3D::updateTessellation(view, look);

    // Use emissivity to indicate selectedness
    if (selected && manualControl) {
        for (index_t i = 0; i < materialCount(); i++)
            material(i)->emissivity = MANUAL_UNIT_EMISSIVITY;
    } else if (selected) {
        for (index_t i = 0; i < materialCount(); i++)
            material(i)->emissivity = SELECTED_UNIT_EMISSIVITY;
    } else {        
        for (index_t i = 0; i < materialCount(); i++)
            material(i)->emissivity = UNSELECTED_UNIT_EMISSIVITY;
    }


    // Make sure our little target thingy get's cleaned up
    Tessellation &tess = *tessellation;
    Tessellation::LineGroup &lineGroup = tess.lineGroup(goalMaterialIndex);
    lineGroup.clear();

    // Redraw it if we're supposed to
    if (target != NULL && renderGoal) {
        const Transform::Quaternion &mRotation = transform->rotation();
        const Transform::Quaternion &tRotation = target->transform->rotation();
        Transform::Point goal(mRotation.unrotate(goalDisplacement));

        if (selected)
            cerr << "Goal is " << goal << endl;

        const Transform::Vector &scale = transform->scale();
        index_t vtx[GOAL_VERTEX_COUNT + 2];
        Transform::Point pt;
        for (index_t i = 0; i < GOAL_VERTEX_COUNT + 2; i++) {
            // Make the goal gadget rotate with the target
            pt = tRotation.rotate(mRotation.unrotate(GOAL_VERTICES[i]));
            for (index_t j = 0; j < 3; j++)
                pt[j] = (goal[j] + pt[j]) / scale[j];
            vtx[i] = tess.addVertex(pt);
        }

        vtx[GOAL_VERTEX_COUNT] = tess.addVertex(Transform::Point(0.0));
        vtx[GOAL_VERTEX_COUNT + 1] = tess.addVertex(Transform::Point(goalDisplacement) * -1.0);

        Tessellation::Line line;
        // Circle
        line.v[0] =  vtx[0];  line.v[1] =  vtx[1];  lineGroup.push_back(line);
        line.v[0] =  vtx[1];  line.v[1] =  vtx[2];  lineGroup.push_back(line);
        line.v[0] =  vtx[2];  line.v[1] =  vtx[3];  lineGroup.push_back(line);
        line.v[0] =  vtx[3];  line.v[1] =  vtx[4];  lineGroup.push_back(line);
        line.v[0] =  vtx[4];  line.v[1] =  vtx[5];  lineGroup.push_back(line);
        line.v[0] =  vtx[5];  line.v[1] =  vtx[6];  lineGroup.push_back(line);
        line.v[0] =  vtx[6];  line.v[1] =  vtx[7];  lineGroup.push_back(line);
        line.v[0] =  vtx[7];  line.v[1] =  vtx[0];  lineGroup.push_back(line);

        // Major ticks
        line.v[0] =  vtx[8];  line.v[1] =  vtx[9];  lineGroup.push_back(line);
        line.v[0] = vtx[10];  line.v[1] = vtx[11];  lineGroup.push_back(line);
        line.v[0] = vtx[12];  line.v[1] = vtx[13];  lineGroup.push_back(line);
        line.v[0] = vtx[14];  line.v[1] = vtx[15];  lineGroup.push_back(line);

        // Minor ticks
        line.v[0] = vtx[16];  line.v[1] = vtx[17];  lineGroup.push_back(line);
        line.v[0] = vtx[18];  line.v[1] = vtx[19];  lineGroup.push_back(line);
        line.v[0] = vtx[20];  line.v[1] = vtx[21];  lineGroup.push_back(line);
        line.v[0] = vtx[22];  line.v[1] = vtx[23];  lineGroup.push_back(line);

        // Displacement line
//        line.v[0] = vtx[24];  line.v[1] = vtx[25];  lineGroup.push_back(line);
    }
}


// APC specialization
APC::APC() : BattleUnit(OBJ(cube.obj)) {
    mass = 300.0;
    maxEngineForce = 200.0;
    minTurningRadius = 1.0;
    axleOffset = 0.5;

    Transform::Vector scale(0.15, 0.15, 0.2);
    elevationOffset = scale[1]; 
    transform->scale(scale);

    addMaterial(MaterialPtr(new Material()));
    material(0)->diffuse = Material::Color(0.1f, 0.4f, 1.0f);

    // Tack on our material for drawing targets
    MaterialPtr goalMat(new Material());
    goalMat->illuminated = false;
    goalMat->diffuse = GOAL_COLOR;
    goalMaterialIndex = materialCount();
    addMaterial(goalMat);
}


// Humvee specialization
Humvee::Humvee() : BattleUnit(OBJ(cube.obj)) {
#if 0
    mass = 500.0;
    maxEngineForce = 500.0;
    minTurningRadius = 0.5;
    axleOffset = 0.5;
    maxAngularFriction = 0.8;

    Transform::Vector scale(0.15, 0.05, 0.25);
#else
    mass = 100.0;
    axleOffset = 0.6;
    maxEngineForce = 150.0;
    minTurningRadius = 0.5;

    Transform::Vector scale(0.15, 0.05, 0.25);
#endif
    elevationOffset = scale[1]; 
    transform->scale(scale);

    addMaterial(MaterialPtr(new Material()));
    material(0)->diffuse = Material::Color(0.6f, 0.6f, 0.0f);

    // Tack on our material for drawing targets
    MaterialPtr goalMat(new Material());
    goalMat->illuminated = false;
    goalMat->diffuse = GOAL_COLOR;
    goalMaterialIndex = materialCount();
    addMaterial(goalMat);
}


// LightTank specialization
LightTank::LightTank() : BattleUnit(OBJ(cube.obj)) {
    mass = 300.0;
    maxEngineForce = 500.0;
    minTurningRadius = 1.0;

    Transform::Vector scale(0.2, 0.1, 0.3);
    elevationOffset = scale[1]; 
    transform->scale(scale);

    addMaterial(MaterialPtr(new Material()));
    material(0)->diffuse = Material::Color(0.8f, 0.0f, 0.0f);

    // Tack on our material for drawing targets
    MaterialPtr goalMat(new Material());
    goalMat->illuminated = false;
    goalMat->diffuse = GOAL_COLOR;
    goalMaterialIndex = materialCount();
    addMaterial(goalMat);
}


// HeavyTank specialization
HeavyTank::HeavyTank() : BattleUnit(OBJ(cube.obj)) {
    mass = 500.0;
    maxEngineForce = 500.0;
    minTurningRadius = 0.5;
    axleOffset = 0.5;
    maxAngularFriction = 0.8;

    Transform::Vector scale(0.25, 0.2, 0.3);
    elevationOffset = scale[1]; 
    transform->scale(scale);

    addMaterial(MaterialPtr(new Material()));
    material(0)->diffuse = Material::Color(0.0f, 0.8f, 0.0f);

    // Tack on our material for drawing targets
    MaterialPtr goalMat(new Material());
    goalMat->illuminated = false;
    goalMat->diffuse = GOAL_COLOR;
    goalMaterialIndex = materialCount();
    addMaterial(goalMat);
}
