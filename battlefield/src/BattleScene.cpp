/*
 * File: BattleField.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      This file implements the BattleField class, defined in BattleField.hpp.
 */

// Import class definition
#include "BattleScene.hpp"

// Import other Battlefield classes
#include "BattleUnitControl.hpp"
#include "GroundConstraint.hpp"
using namespace Battlefield;


// Shortcut for constructing filenames
#define IMG(NAME) string("data\\" #NAME)


// Ground plane properties
Transform::scalar_t FIELD_WIDTH     = 10.0;
Transform::scalar_t FIELD_LENGTH    = 10.0;
Transform::scalar_t FIELD_ELEVATION = 0.0;

Material::Color FIELD_AMBIENT(0.2f, 0.2f, 0.2f);
Material::Color FIELD_DIFFUSE(0.8f, 0.8f, 0.8f);
Material::Color FIELD_SPECULAR(0.0f, 0.0f, 0.0f);
string FIELD_TEXTURE = IMG(mud01.jpeg);
Material::scalar_t FIELD_TEXTURE_REPEAT_U = 1.0f;
Material::scalar_t FIELD_TEXTURE_REPEAT_V = 1.0f;

// Sun properties
Transform::Point   SUN_LOCATION(100.0, 100.0, 100.0);
Material::Color    SUN_COLOR(1.0f, 1.0f, 0.8f);
Material::scalar_t SUN_INTENSITY = 1.0f;

// Physical rules
RigidBody::Vector   GRAVITY(0.0, -9.8, 0.0);

typedef SolidObject3D::LinearApproximation PolygonMesh;
typedef SolidObject3D::LinearApproximationPtr PolygonMeshPtr;


// Constructor
BattleScene::BattleScene() {
    // Configure the rigid-body simulator
    system = RigidBodySystemPtr(new RigidBodySystem(0.0));

    // We should, of course, have gravity to keep us on the ground
    SecondDerivOpPtr op2 = new SimpleGravityForce(GRAVITY);
    system->add(op2);

    // We want to ensure that we don't penetrate the ground
    ZerothDerivOpPtr op0 = new GroundConstraint(FIELD_ELEVATION);
    system->add(op0);

    // Build the mesh that represents the ground
    PolygonMeshPtr mesh(new PolygonMesh());
    PolygonMesh::VertexPtr v[4];
    v[0] = mesh->createVertex(PolygonMesh::Point(-FIELD_LENGTH / 2.0, FIELD_ELEVATION, -FIELD_WIDTH / 2.0));
    v[1] = mesh->createVertex(PolygonMesh::Point(FIELD_LENGTH / 2.0, FIELD_ELEVATION, -FIELD_WIDTH / 2.0));
    v[2] = mesh->createVertex(PolygonMesh::Point(FIELD_LENGTH / 2.0, FIELD_ELEVATION, FIELD_WIDTH / 2.0));
    v[3] = mesh->createVertex(PolygonMesh::Point(-FIELD_LENGTH / 2.0, FIELD_ELEVATION, FIELD_WIDTH / 2.0));
    index_t normal = mesh->createNormal(PolygonMesh::Vector(0.0, 1.0, 0.0));
    index_t tex[4];
    tex[0] = mesh->createTexCoords(PolygonMesh::TexCoords(0.0, 0.0));
    tex[1] = mesh->createTexCoords(PolygonMesh::TexCoords(FIELD_TEXTURE_REPEAT_U, 0.0));
    tex[2] = mesh->createTexCoords(PolygonMesh::TexCoords(FIELD_TEXTURE_REPEAT_U, FIELD_TEXTURE_REPEAT_V));
    tex[3] = mesh->createTexCoords(PolygonMesh::TexCoords(0.0, FIELD_TEXTURE_REPEAT_V));
    PolygonMesh::FaceVertexPtrList vertices;
    vertices.push_back(mesh->createFaceVertex(v[0]));
    vertices[0]->setNormalIndex(normal);
    vertices[0]->setTexCoordsIndex(tex[0]);
    vertices.push_back(mesh->createFaceVertex(v[3]));
    vertices[1]->setNormalIndex(normal);
    vertices[1]->setTexCoordsIndex(tex[3]);
    vertices.push_back(mesh->createFaceVertex(v[2]));
    vertices[2]->setNormalIndex(normal);
    vertices[2]->setTexCoordsIndex(tex[2]);
    mesh->createFace(vertices);
    vertices.clear();
    vertices.push_back(mesh->createFaceVertex(v[0]));
    vertices[0]->setNormalIndex(normal);
    vertices[0]->setTexCoordsIndex(tex[0]);
    vertices.push_back(mesh->createFaceVertex(v[2]));
    vertices[1]->setNormalIndex(normal);
    vertices[1]->setTexCoordsIndex(tex[2]);
    vertices.push_back(mesh->createFaceVertex(v[1]));
    vertices[2]->setNormalIndex(normal);
    vertices[2]->setTexCoordsIndex(tex[1]);
    mesh->createFace(vertices);

    // Construct the material properties
    MaterialPtr groundMat(new Material());
    groundMat->illuminated = true;
    groundMat->setImage(FIELD_TEXTURE);
    groundMat->ambient = FIELD_AMBIENT;
    groundMat->diffuse = FIELD_DIFFUSE;
    groundMat->specular = FIELD_SPECULAR;

    // Put it all together
    groundPlane = SolidObject3DPtr(new SolidObject3D());
    groundPlane->addApproximation(mesh);
    groundPlane->addMaterial(groundMat);

    // Create the sun
    sunLight = LightPtr(new Light());
    sunLight->color = SUN_COLOR;
    sunLight->intensity = SUN_INTENSITY;
    sunLight->transform->setLocationPoint(new Transform::Point(SUN_LOCATION));

    // Stick it all in the scene
    addObject(groundPlane);
    addLight(sunLight);
}

void BattleScene::addBattleUnit(BattleUnitPtr unit) {
    // Add this to the Scene superclass
    ObjectPtr obj = static_pointer_cast<Object>(unit);
    addObject(obj);

    // Make sure we've got one selected
    if (battleUnitCount() == 0)
        unit->selected = true;

    // Put it into the rigid body system
    SolidObject3DPtr s3o = static_pointer_cast<SolidObject3D>(unit);
    RigidBodyPtr rb = new RigidBody(s3o, unit->mass);
    unit->rigidBody = shared_ptr<RigidBody>(rb);
    system->add(rb);
    BattleUnitControl * buc(new BattleUnitControl(unit, rb));
    system->add(static_cast<ThirdDerivOp *>(buc));
    system->add(static_cast<SecondDerivOp *>(buc));

    // Stick it in our special list of units
    ADD_VALUE(battleUnit, unit);
}

BattleUnitPtr BattleScene::addBattleUnit(const string &type) {
    BattleUnitPtr bu;
    try {
        if (type == "APC")
            bu = BattleUnitPtr(new APC());
        else if (type == "humvee")
            bu = BattleUnitPtr(new Humvee());
        else if (type == "light tank")
            bu = BattleUnitPtr(new LightTank());
        else if (type == "heavy tank")
            bu = BattleUnitPtr(new HeavyTank());
        else
            cerr << "addBattleUnit(\"" << type << "\"): Unsupported unit type\n";

        // If we were successful, stick it in
        if (bu != NULL)
            addBattleUnit(bu);

    } catch (const Inca::IncaException &e) {    // Boom.
        cerr << "addBattleUnit(\"" << type << "\"): " << e << '\n';
    }

    return bu;
}

// Formation creation functions
void BattleScene::createRow(BattleUnitPtr leader,
                            const string &unitType,
                            const Vector &offset,
                            size_t number) {
    const Quaternion &leaderRotation = leader->transform->rotation();
    Vector relOffset = leaderRotation.rotate(offset);
    Vector relFromLeader = relOffset;
    Vector absFromLeader = offset;
    for (size_t i = 0; i < number; i++) {
        BattleUnitPtr unit = addBattleUnit(unitType);
        unit->target = leader;
        unit->targetOffset = absFromLeader;
        unit->relativeOffset = true;
        unit->transform->setRotation(leaderRotation);
        unit->transform->setLocationPoint(
            new Point(*leader->transform->locationPoint() + relFromLeader));

        // Advance down the line
        absFromLeader += offset;
        relFromLeader += relOffset;
    }
}

void BattleScene::createDiamond(BattleUnitPtr leader,
                                const string &unitType,
                                const Vector &offset) {
    const Quaternion &leaderRotation = leader->transform->rotation();
    Vector relOffset = leaderRotation.rotate(offset);
    Vector absReflected = reflect(offset, Vector(0.0, 0.0, -1.0));
    Vector relReflected = leaderRotation.rotate(absReflected);
    Vector relFromLeader, absFromLeader;
    for (size_t i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            relFromLeader = relOffset;
            absFromLeader = offset;
            break;
        case 1:
            relFromLeader = relReflected;
            absFromLeader = absReflected;
            break;
        case 2:
            relFromLeader = relOffset + relReflected;
            absFromLeader = offset + absReflected;
            break;
        }
        BattleUnitPtr unit = addBattleUnit(unitType);
        unit->target = leader;
        unit->targetOffset = absFromLeader;
        unit->relativeOffset = true;
        unit->transform->setRotation(leaderRotation);
        unit->transform->setLocationPoint(
            new Point(*leader->transform->locationPoint() + relFromLeader));
    }
}

void BattleScene::createTriangle(BattleUnitPtr leader,
                                 const string &unitType,
                                 const Vector &offset) {
    const Quaternion &leaderRotation = leader->transform->rotation();
    Vector relOffset = leaderRotation.rotate(offset);
    Vector absReflected = reflect(offset, Vector(0.0, 0.0, -1.0));
    Vector relReflected = leaderRotation.rotate(absReflected);
    Vector relFromLeader, absFromLeader;
    for (size_t i = 0; i < 2; i++) {
        switch (i) {
        case 0:
            relFromLeader = relOffset;
            absFromLeader = offset;
            break;
        case 1:
            relFromLeader = relReflected;
            absFromLeader = absReflected;
            break;
        }
        BattleUnitPtr unit = addBattleUnit(unitType);
        unit->target = leader;
        unit->targetOffset = absFromLeader;
        unit->relativeOffset = true;
        unit->transform->setRotation(leaderRotation);
        unit->transform->setLocationPoint(
            new Point(*leader->transform->locationPoint() + relFromLeader));
    }
}

void BattleScene::update(double time) {
    system->update(time);
}