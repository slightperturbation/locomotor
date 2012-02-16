/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// Copyright (C) 2004, Brian F. Allen, 
//
// Contact: vector@acm.org

// Project Includes
#include <ArticulatedBody.hpp>
#include <Renderer.hpp>

// External Includes
#include <fstream>

using namespace Else;

Else::ArticulatedBody
::ArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID )
:   myWorldID( argWorldID ), 
    mySpaceID( argSpaceID ),
    myHingeMotorType( DEFAULT_HINGE_MOTOR_TYPE ),
    myOverrideActuatorID( -1 ), //< disabled
    myOverrideActuatorSetting( 0 ),
    myForceScale( DEFAULT_FORCE_SCALE ),
    myHasController( true ),
    myHarnessHeight( DEFAULT_HARNESS_HEIGHT ),
    myAppliedHarness( 0.0 ),
    myIsUnstable( false ),
    myJointGroupID( 0 )
{
    // Subclasses are responsible for creating actual ode bodies
    // subclasses *must* initialize myActuatorActivation before first use (size is body-type dependent)
}

Else::ArticulatedBody
::~ArticulatedBody()
{
    clear();
}

void 
Else::ArticulatedBody
::acquireState( std::vector< std::vector< BodyState > >& outStates )
{
    std::vector< BodyState > frame;
    BodyMap::iterator biter = myBodies.begin();
    size_t bodyCount = myBodies.size();
    frame.resize( bodyCount );
    for( size_t i = 0; i < bodyCount; ++i )
    {
        getBodyState( (BodySegmentEnum)i, frame[i] );
    }
    outStates.push_back( frame );
}

void 
Else::ArticulatedBody
::setState( const std::vector< BodyState >& argStates )
{
    std::vector< BodyState >::const_iterator iter = argStates.begin();
    for( ; iter != argStates.end(); ++iter )
    {
        setBodyState( *iter );
    }
}


void
Else::ArticulatedBody
::getBodyState( Else::BodySegmentEnum argBody, BodyState& out )
{
    out.id = argBody;
    dBodyID bodyID = myBodies[ argBody ];
    const dReal* pos = dBodyGetPosition( bodyID );
    for( size_t i = 0; i < 3; ++i )
    {
        out.pos[i] = pos[i];
    }
    
    const dReal* rot = dBodyGetQuaternion( bodyID );
    for( size_t i = 0; i < 4; ++i )
    {
        out.rot[i] = rot[i];
    }
    
    const dReal* vel = dBodyGetLinearVel( bodyID );
    for( size_t i = 0; i < 3; ++i )
    {
        out.vel[i] = vel[i];
    }
    
    const dReal* angvel = dBodyGetAngularVel( bodyID );
    for( size_t i = 0; i < 3; ++i )
    {
        out.angvel[i] = angvel[i];
    }
}

void
Else::ArticulatedBody
::setBodyState( const BodyState& argBodyState )
{
    dBodyID bodyID = myBodies[ argBodyState.id ];
    dBodySetPosition( bodyID, argBodyState.pos[0], argBodyState.pos[1], argBodyState.pos[2] );
    dQuaternion quat;
    for( size_t i=0;i<4;++i) { quat[i]=argBodyState.rot[i]; }
    dBodySetQuaternion( bodyID, quat );
    dBodySetLinearVel( bodyID, argBodyState.vel[0], argBodyState.vel[1], argBodyState.vel[2] );
    dBodySetAngularVel( bodyID, argBodyState.angvel[0], argBodyState.angvel[1], argBodyState.angvel[2] ); 
}
void
Else::ArticulatedBody
::setBodyState( const std::vector< BodyState >& argBodyStates )
{    
    for( std::vector< BodyState >::const_iterator biter = argBodyStates.begin(); biter != argBodyStates.end(); ++biter )
    {
        const BodyState& bs = *biter;
        setBodyState( bs );            
    }
}
void
Else::ArticulatedBody
::render( laetoli::Renderer* argRenderer )
{
    BodyMap::iterator biter = myBodies.begin();
    BodyDimensions::iterator iter = myBodyDims.begin();
    while( biter != myBodies.end() && iter != myBodyDims.end() )
    {
        const dReal* rot = dBodyGetRotation( (*biter).second );
        const dReal* pos = dBodyGetPosition( (*biter).second );
        //dVector3 rawDim;
        //dGeomBoxGetLengths( argGeomID, rawDim );
        //laetoli::Vec3d dim( rawDim[0], rawDim[1], rawDim[2] );
        gmtl::Vec3d s = (*iter).second;
        laetoli::Vec3d ls( s[0], s[1], s[2] );
        
        double matrix[16];
        
        matrix[0] = rot[0];
        matrix[1] = rot[4];
        matrix[2] = rot[8];
        matrix[3] = 0;
        matrix[4] = rot[1];
        matrix[5] = rot[5];
        matrix[6] = rot[9];
        matrix[7] = 0;
        matrix[8] = rot[2];
        matrix[9] = rot[6];
        matrix[10] = rot[10];
        matrix[11] = 0;
        matrix[12] = pos[0];
        matrix[13] = pos[1];
        matrix[14] = pos[2];
        matrix[15] = 1;
        
        argRenderer->pushMatrix();
        {
            argRenderer->multiplyMatrix( matrix );
            //argRenderer->drawBox( dim.toArray() );
            argRenderer->drawBox( ls.toArray() );
        }
        argRenderer->popMatrix();
        
        biter++; iter++;
    }
}

dBodyID
Else::ArticulatedBody
::createVerticalSegment( BodySegmentEnum argSegmentName,
                        double argLateralDist, 
                        double argCenter, 
                        double argWidth, 
                        double argHeight, 
                        double argWeight,
                        double argLength,
                        double argLengthOffset ) 
{
    assert( argWeight > 0.001 );
    assert( argWidth > 0.001 );
    assert( argHeight > 0.001 );
	double yOffset = 0;
	
    if( std::fabs( argLength ) < 0.001 )
    {
        // default lenght is width
        argLength = argWidth;
    }
    else
    {
        yOffset = argLength / 4;
    }
    
    dBodyID bodyID = dBodyCreate( myWorldID );
    dBodySetData( bodyID, this );
    dGeomID geomID = dCreateBox( mySpaceID, argWidth, argLength, argHeight );
    dGeomSetBody( geomID, bodyID );
    dGeomSetData( geomID, (void*)argSegmentName );
    dMass m;
    dMassSetBoxTotal( &m, argWeight, argWidth, argLength, argHeight );
    dBodySetMass( bodyID, &m );
    
    dBodySetPosition( bodyID, argLateralDist, yOffset + argLengthOffset, argCenter );
    dGeomSetPosition( geomID, argLateralDist, yOffset + argLengthOffset, argCenter );
    
    myBodies[argSegmentName] = bodyID;
    myBodyDims[argSegmentName] = gmtl::Vec3d( argWidth, argLength, argHeight );
    myGeoms.push_back( geomID );
    
    return bodyID;
}


void 
Else::ArticulatedBody
::addHarnessForce( double argStrength, double argTimeStep )
{
    if( argStrength > 1 || argStrength < 0 )
    {
        std::cerr << "Error with argStrength in harness: "
        << argStrength << "\n";
        return;
    }
	if( argStrength < 0.01 )
	{
		myAppliedHarness = 0.0;
		return;
	}
    
    dBodyID bod = myBodies[Trunk];
    dMass dm;
    dReal m = 0;
	dReal totForce[] = { 0, 0, 0 };
	const dReal* currForce;
    
    const dReal* pos = dBodyGetPosition( bod );
    if( pos[2] < myHarnessHeight )
    {
        const dReal* linearVel = dBodyGetLinearVel( bod );
		
		// Need to support entire structure
        BodyMap::iterator iter = myBodies.begin();
        while( iter != myBodies.end() )
        {
            dBodyGetMass( (*iter).second, &dm );
            m += dm.mass;
            
			currForce = dBodyGetForce( (*iter).second );
			totForce[0] += currForce[0];
			totForce[1] += currForce[1];
			totForce[2] += currForce[2];
            iter++;
        }
        
        // f = m * dv/dt 
        // vertical
		dReal fz = 0;
		if( linearVel[2] < 0 )
		{
			//  -5.0 
            // f = m * dv/dt 
			fz = DEFAULT_HARNESS_VERTICAL_FACTOR * dWorldGetERP( myWorldID ) * argStrength * m * linearVel[2] / argTimeStep;
            // f = m * g
			fz += DEFAULT_HARNESS_VERTICAL_FACTOR * m * DEFAULT_GRAVITY;
		}
        
        // Small forward resistance
		dReal fy = 0;
		if( linearVel[1] < 0 )
		{
			// going backwards, do nothing (let fall on ass)
			//const double factor = -0.9;
			fy = 0.0; //factor * dWorldGetERP( myWorldID ) * argStrength * m * linearVel[1] / argTimeStep;
		}
		else
		{
			// gentle push forward
			const double velError = DEFAULT_HARNESS_FORWARD_VEL - linearVel[1];
			fy = DEFAULT_HARNESS_FORWARD_FACTOR 
            * dWorldGetERP( myWorldID ) 
            * argStrength 
            * m 
            * velError 
            / argTimeStep;
		}
        
        // sagital
        dReal fx = -1.0 * DEFAULT_HARNESS_SAGITAL_FACTOR * dWorldGetERP( myWorldID ) * argStrength * m * linearVel[0] / argTimeStep;
		
		dBodyAddForce( bod, fx, fy, fz);
        
		myAppliedHarness += fabs(fz);
		//myAppliedHarness += fabs(fx) + fabs(fy) + fabs(fz);
    }
    
    // Angular Damp
    const dReal *w = dBodyGetAngularVel( bod );
    dReal torque[3];
    dMatrix3 I1, I_gbl;
    dBodyGetMass( bod, &dm );
    const dReal *R = dBodyGetRotation( bod );
    
    // I_gbl = R * dm.I * R^t
    dMULTIPLY0_333( I1, R, dm.I );
    dMULTIPLY2_333( I_gbl, I1, R );
    
    // torque = I_gbl * w / dt
    dMULTIPLY0_331( torque, I_gbl, w );
    
	//const double angularFactor = -0.5;
	const double angularFactor = -1.5;
    for( int i = 0; i < 3; ++i )
    {
        torque[i] *= angularFactor * argStrength / argTimeStep;
    }
    
	// Here we are only damping rotation around the vertical axis and the darko axis
	// but also adding a cost for it
    dBodyAddTorque( bod, 0, torque[1], torque[2] );
	myAppliedHarness += fabs( torque[2] ) + fabs( torque[1] );
}

gmtl::Quatd
Else::ArticulatedBody
::getOrientation(void)
{
    assert( false );
    // TODO
    const dReal* r = dBodyGetRotation( myBodies[Waist] );
    dQuaternion q;
    dRtoQ( r, q );
    
    return gmtl::Quatd( q[0], q[1], q[2], q[3] );
}


void
Else::ArticulatedBody
::recordCollision( BodySegmentEnum argCollidingBodySegment, double argTime )
{
    const dReal* pos = dBodyGetPosition( myBodies[argCollidingBodySegment] );
    
    myCollisionPoints[ argCollidingBodySegment ] = gmtl::Point3d( pos[0], pos[1], pos[2] );
    myCollisionTimes[ argCollidingBodySegment ] = argTime;
}

gmtl::Point3d
Else::ArticulatedBody
::getLastCollision( BodySegmentEnum argBody )
{
    CollisionPointMap::iterator iter = myCollisionPoints.find( argBody );
    if( iter == myCollisionPoints.end() )
    {
        return gmtl::Point3d( 0, 0, 0 );
    } 
    else
    {
        return (*iter).second;
    }
}


SideEnum
Else::ArticulatedBody
::getSideOfLastCollision( void )
{
    double timeR = -1.0;
    CollisionTimeMap::iterator iter = myCollisionTimes.find( RightFoot );
    if( iter != myCollisionTimes.end() )
    {
        timeR = (*iter).second;
    }
    double timeL = -1.0;
    iter = myCollisionTimes.find( LeftFoot );
    if( iter != myCollisionTimes.end() )
    {
        timeL = (*iter).second;
    }
    if( timeR > timeL )
    {
        return RightSide;
    }
    return LeftSide;
}


gmtl::Point3d
Else::ArticulatedBody
::getPointOfLastFootCollision( void )
{
    SideEnum side = getSideOfLastCollision();
    if( RightSide == side )
    {
        return getLastCollision( RightFoot );
    }
    else
    {
        return getLastCollision( LeftFoot );
    }		
}

bool
Else::ArticulatedBody
::hasNonFootCollision( void )
{
    BodyMap::iterator bodIter = myBodies.begin();
    while( bodIter != myBodies.end() )
    {
        if( (*bodIter).first != RightFoot 
           && (*bodIter).first != LeftFoot  
           )
        {
            CollisionPointMap::iterator iter = myCollisionPoints.find( (*bodIter).first );
            if( iter != myCollisionPoints.end() ) 
            {
                std::cout << " COL(" << (*bodIter).first << ") ";
                return true;
            }
        }
        ++bodIter;
    }
    return false;
}


void
Else::ArticulatedBody
::manuallyOverrideActuator( int argActuatorID, double argSetting )
{
    myOverrideActuatorID = argActuatorID;
    myOverrideActuatorSetting = argSetting;
}


void
Else::ArticulatedBody
::reset( void )
{
    clear();
    createBody();
}

void
Else::ArticulatedBody
::clear( void )
{
    BodyMap::iterator iter = myBodies.begin();
    while( iter != myBodies.end() )
    {
        dBodyDestroy( (*iter).second );
        iter++;
    }
    for( size_t i = 0; i < myGeoms.size(); ++i )
    {
        dGeomDestroy( myGeoms[i] );
    }
    JointMap::iterator jiter = myJoints.begin();
    while( jiter != myJoints.end() )
    {
        dJointDestroy( (*jiter).second );
        ++jiter;
    }
    JointMap::iterator miter = myMotors.begin();
    while( miter != myMotors.end() )
    {
        dJointDestroy( (*miter).second );
        ++miter;
    }
    myAppliedHarness = 0.0;
    myBodies.clear();
    myGeoms.clear();
    myJoints.clear();
    myMotors.clear();
    myBodyDims.clear();
    myCollisionPoints.clear();
    myCollisionTimes.clear();
}


void
Else::ArticulatedBody
::render( OpenGLRenderer* argRenderer )
{
#ifdef COCOA_GUI
    assert( myBodies.size() == myBodyDims.size() );
    BodyMap::iterator biter = myBodies.begin();
    BodyDimensions::iterator iter = myBodyDims.begin();
    while( biter != myBodies.end() && iter != myBodyDims.end() )
    {
        const dReal* p = dBodyGetPosition( (*biter).second );
        gmtl::Vec3d pos( p[0], p[1], p[2] );
        const dReal* rot = dBodyGetRotation( (*biter).second );
        argRenderer->drawBox( pos, (*iter).second, rot );
        iter++; biter++;
    }
    
    dVector3 pos;
    JointMap::iterator i = myJoints.begin();
    while( i != myJoints.end() )
    {
        if( dJointTypeBall == dJointGetType( (*i).second ) )
        {
            dJointGetBallAnchor( (*i).second, pos );
            gmtl::Vec3d drawPos( pos[0], pos[1], pos[2] );
            //glColor3f( 1, 0.5, 0.5 );
            argRenderer->drawMarker( drawPos );
            dJointGetBallAnchor2( (*i).second, pos );
            gmtl::Vec3d drawPos1( pos[0], pos[1], pos[2] );
            //glColor3d( 1, 0.5, 0.5 );
            argRenderer->drawMarker( drawPos1 );
            gmtl::Vec3d diff = drawPos1 - drawPos;
            if( length( diff ) > DEFAULT_STABILITY_THRESHOLD )
            {
                // Unstable!
                std::cerr << " Ball joint " << (*i).second << " diverging by "
                << diff << "\n";
                myIsUnstable = true;
            }
        }
        i++;
    }
#endif
}


bool
Else::ArticulatedBody
::testStability( void )
{
    bool isUnstable = false;
    dVector3 pos;
    JointMap::iterator i = myJoints.begin();
    while( i != myJoints.end() )
    {
        if( dJointTypeBall == dJointGetType( (*i).second ) )
        {
            dJointGetBallAnchor( (*i).second, pos );
            gmtl::Vec3d drawPos( pos[0], pos[1], pos[2] );
            dJointGetBallAnchor2( (*i).second, pos );
            gmtl::Vec3d drawPos1( pos[0], pos[1], pos[2] );
            gmtl::Vec3d diff = drawPos1 - drawPos;
            if( length( diff ) > DEFAULT_STABILITY_THRESHOLD )
            {
                // Unstable!
                myIsUnstable = true;
                break;
            }
        }
        i++;
    }
    isUnstable = myIsUnstable;
    myIsUnstable = false; // only return false once for a divergence
    return isUnstable;
}


double
Else::ArticulatedBody
::scaleActuator( double argControllerOut, double argMin, double argMax )
{
    double retval = ( argMax - argMin ) * argControllerOut + argMin;
    return retval;
}


double
Else::ArticulatedBody
::computePD( double argCurrPos,
            double argCurrVel,
            double argDesiredPos,
            double k_p, double k_d )
{
    double deltaAngle = argDesiredPos - argCurrPos;
    double f_p = k_p * deltaAngle;
    double f_d = k_d * -1 * argCurrVel; // minus final velocity==0
    
    return f_p + f_d;
}

void
Else::ArticulatedBody
::maintainAMotorAngle(dJointID argJointID,
                      int argAxis,
                      double argDesiredExtension )
{
    using namespace gmtl::Math;
    double k_p = 1;
    double k_d = 0.2;
    static double maxVel = 1000;
    
    assert( argDesiredExtension >= 0.0 && argDesiredExtension <= 1.0 );
    
    double highPos = dJointGetAMotorParam( argJointID, dParamHiStop + (dParamGroup * argAxis) );
    double lowPos = dJointGetAMotorParam( argJointID, dParamLoStop + (dParamGroup * argAxis) );
    
    double desiredAngle = (highPos - lowPos) * argDesiredExtension + lowPos;
    
    double prevPos;
    std::map< dJointID, double >::iterator i = myPrevJointPos.find( argJointID );
    if( i == myPrevJointPos.end() ) 
    {
        myPrevJointPos[ argJointID ] = 0;
    }
    prevPos = myPrevJointPos[ argJointID ];
    
    double currPos = dJointGetAMotorAngle( argJointID, argAxis );
    double currVel = (currPos - prevPos) / myLastTimeStep; //dJointGetAMotorAngleRate( argJointID, argAxis );
    double vel = computePD( currPos, currVel, desiredAngle, k_p, k_d );
    vel = std::min<double>( maxVel, vel );
    dJointSetAMotorParam( argJointID, dParamVel + dParamGroup * argAxis, vel );
}

void
Else::ArticulatedBody
::actuateHinge( dJointID argHingeJointID, double argDesiredNorm )
{
    switch( myHingeMotorType )
    {
        case MotorPos:
            return actuateHingePos( argHingeJointID, argDesiredNorm );
            break;
        case MotorVel:
            return actuateHingeVel( argHingeJointID, argDesiredNorm );
            break;
        case MotorTorque:
            return actuateHingeTorque( argHingeJointID, argDesiredNorm );
            break;
    };
}

void
Else::ArticulatedBody
::actuateHingeVel( dJointID argHingeJointID,
                  double argDesiredNormVelocity )
{
    assert( argDesiredNormVelocity >= 0.0 && argDesiredNormVelocity <= 1.0 );
    
    const double velScale = 6.3; //2pi radians per second
    dJointSetHingeParam( argHingeJointID, dParamVel, argDesiredNormVelocity * velScale );
}

void 
Else::ArticulatedBody
::actuateHingeTorque(dJointID argHingeJointID,
                     double argDesiredNormTorque )
{
    assert( argDesiredNormTorque >= 0.0 && argDesiredNormTorque <= 1.0 );
    //const double hingeTorqueScale = 100.0;  //
    const double maxTorque = dJointGetHingeParam( argHingeJointID,  dParamFMax );
    double torque = maxTorque * ( argDesiredNormTorque + 1.0 ) * 0.5;  
    dJointAddHingeTorque( argHingeJointID, torque );
}

void
Else::ArticulatedBody
::actuateHingePos(dJointID argHingeJointID,
                  double argDesiredExtension )
{
    using namespace gmtl::Math;
    double k_p = 1;
    double k_d = 0.2;
    static double maxVel = 1000;
    
    assert( argDesiredExtension >= 0.0 && argDesiredExtension <= 1.0 );
    //double highPos = 0;  //dJointGetHingeParam( argHingeJointID, dParamHiStop );
    //double lowPos = -2.0 * PI / 3.0; //dJointGetHingeParam( argHingeJointID, dParamLoStop );
    double highPos = dJointGetHingeParam( argHingeJointID, dParamHiStop );
    double lowPos = dJointGetHingeParam( argHingeJointID, dParamLoStop );
    
    //assert( highPos > 0.0001 || highPos < 0.0001 );
    //assert( lowPos > 0.0001 || lowPos < 0.0001 );
    
    double desiredAngle
    = (highPos - lowPos) * argDesiredExtension + lowPos;
    
    double currPos = dJointGetHingeAngle( argHingeJointID );
    double currVel = dJointGetHingeAngleRate( argHingeJointID );
    //double diff = desiredAngle - currPos;
    
    double vel = computePD( currPos, currVel, desiredAngle, k_p, k_d );
    vel = std::min<double>( maxVel, vel );
    double kneeVelScale = 75.0;
    dJointSetHingeParam( argHingeJointID, dParamVel, vel * kneeVelScale );
    
    /*
     // implement joint limits
     bool isAtJointLimit = false;
     double torque = 0;
     if( currPos < lowPos )
     {
     diff = lowPos - currPos;
     isAtJointLimit = true;
     }
     if( currPos > highPos )
     {
     diff = highPos - currPos;
     isAtJointLimit = true;
     }
     if( isAtJointLimit )
     {
     // Use exponential springs for joint limits
     double expSpringStiff = 2.0;
     double k_p_exp = 10.0;
     double k_d_exp = 10.0;
     torque = 
     ( k_p_exp * std::exp( expSpringStiff * diff ) - 1.0 ) - k_d_exp * currVel;
     dJointAddHingeTorque( argHingeJointID, torque * myForceScale * myWeight );
     }
     else
     {
     torque = computePD( currPos, currVel, desiredAngle, k_p, k_d );
     //vel = std::min<double>( maxVel, vel );
     //dJointSetHingeParam( argHingeJointID, dParamVel, vel * myKneeVelScale );
     dJointAddHingeTorque( argHingeJointID, torque * myForceScale * myWeight );
     }
     */
}


