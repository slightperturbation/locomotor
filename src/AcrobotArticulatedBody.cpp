#include "AcrobotArticulatedBody.hpp"

#include "Controller.hpp"

Else::AcrobotArticulatedBody
::AcrobotArticulatedBody( dWorldID argWorldID,
                          dSpaceID argSpaceID,
                          double argScale )
: ArticulatedBody( argWorldID, argSpaceID ),
  myRadius( 0.1 * argScale ),
  myLength( argScale ),
  myJointGroupID( 0 )
{
    // create body
    gmtl::Vec3d dims( 2*myRadius, 2*myRadius, 2*myRadius + myLength );
    const double density = 1.0;
    const int zDirection = 3;
    const double extraSpace = 0.1*myLength;
    dMass mass;

    dBodyID body = dBodyCreate( myWorldID );
    dBodySetData( body, this );

    // create cylinder aligned to z-axis
    dGeomID geom = dCreateCapsule( mySpaceID, myRadius, myLength );
    dGeomSetBody( geom, body );
    dBodySetPosition( body, 0, 0, extraSpace + 0.5*myLength + myRadius );
    dMassSetCappedCylinder( &mass, density, zDirection, myRadius, myLength );
    dBodySetMass( body, &mass );

    dJointID joint = dJointCreateHinge( myWorldID, myJointGroupID );
    dJointAttach( joint, NULL, body );
    dJointSetHingeAnchor( joint, 0, 0, extraSpace + myLength + myRadius );
    dVector3 axis;
    axis[0] = 0; axis[1] = 1; axis[2] = 0;
    dJointSetHingeAxis( joint, axis[0], axis[1], axis[2] );

    myBodies[0] = body;
    myJoints[0] = joint;
    myBodyDims[0] = dims;
    myGeoms.push_back( geom );
}

Else::AcrobotArticulatedBody
::~AcrobotArticulatedBody()
{
    clear();
}

double
Else::AcrobotArticulatedBody
::currentAngle( void )
{
    return dJointGetHingeAngle( myJoints[0] );
}

double
Else::AcrobotArticulatedBody
::currentVel( void )
{
    return dJointGetHingeAngleRate( myJoints[0] );
}

/// Fills outSensors with the data gathered from the sensors.
void
Else::AcrobotArticulatedBody
::getSensors( ControllerPtr argController, double argDeltaTime )
{
    argController->setInput( 1, 0, currentAngle() );
    argController->setInput( 1, 1, currentVel() );
}

/// Provides new values that the body should use to control it's actuators.
void
Else::AcrobotArticulatedBody
::setActuators( const ControllerPtr argController )
{
    // 1 actuator
    double torque = 0.85 * scaleTorque( argController->getOutput(1, 0) );
    dJointAddHingeTorque( myJoints[0], torque );
}


void
Else::AcrobotArticulatedBody
::writeCurrJointTraces( double argT )
{
    // TODO
}

void
Else::AcrobotArticulatedBody
::acquireJointAngleData( std::vector< double >& out )
{
    out.push_back( dJointGetHingeAngle( myJoints[0] ) );
}

double
Else::AcrobotArticulatedBody
::scaleTorque( double argTorque )
{
    assert( argTorque <= 1.0 && argTorque >= -1.0 );
    dMass mass;
    dBodyGetMass( myBodies[0], &mass );
    double I_center = 0.5*mass.mass*myRadius*myRadius;
    double I = I_center + mass.mass*myLength*myLength;
    double torque = I * argTorque;
    return torque;
}

void
Else::AcrobotArticulatedBody
::displayMass( void )
{
    dMass mass;
    dBodyGetMass( myBodies[0], &mass );
    std::cerr << "Mass, kg = " << mass.mass << "\n"
            << "\tCenter: "
            << mass.c[0] << ", "
            << mass.c[1] << ", "
            << mass.c[2] << "\n";
}
