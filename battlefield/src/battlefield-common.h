#ifndef BATTLEFIELD_COMMON
#define BATTLEFIELD_COMMON

// Import system configuration
#include <inca-common.h>


// Import Inca sub-libraries
#include <math/Math.h++>
#include <imaging/Imaging.h++>

#include <world/World.h++>
#include <poly/Poly.h++>

#include <rendering/Rendering.h++>
#include <io/IOUtilities.h++>

#include <interface/generic/Framework.h++>
#include <interface/generic/CameraControl.h++>
#include <interface/generic/SceneView.h++>


// Configure defaults
#include <inca-config-math.h>
#include <inca-config-dynamics.h>


// Import namespaces
using namespace Inca;
using namespace Inca::World;
using namespace Inca::Interface;
using namespace Inca::Rendering;


// Do extra typedefs
typedef RigidBodySystem::ThirdDerivativeOperator  ThirdDerivOp;
typedef RigidBodySystem::SecondDerivativeOperator SecondDerivOp;
typedef RigidBodySystem::FirstDerivativeOperator  FirstDerivOp;
typedef RigidBodySystem::ZerothDerivativeOperator ZerothDerivOp;

typedef RigidBodySystem::ThirdDerivativeOperatorPtr  ThirdDerivOpPtr;
typedef RigidBodySystem::SecondDerivativeOperatorPtr SecondDerivOpPtr;
typedef RigidBodySystem::FirstDerivativeOperatorPtr  FirstDerivOpPtr;
typedef RigidBodySystem::ZerothDerivativeOperatorPtr ZerothDerivOpPtr;

#endif
