//
//  HumanoidArticulatedBody.cpp
//  evolverDB
//
//  Created by vector on 8/23/10.
//  Copyright (c) 2010 Brian F. Allen.  All rights reserved.
//

#include <HumanoidArticulatedBody.hpp>

#include <Controller.hpp>


void Else::loadDesc( HumanoidArticulatedBodyDesc& outBodyDesc, const char* argBodyDescFilename )
{
	std::ifstream bodyDescFile( argBodyDescFilename );
	if( bodyDescFile )
	{
		bodyDescFile >> outBodyDesc.height >> outBodyDesc.weight >> outBodyDesc.hipBreadth;
	}
	else
	{
		std::cerr << "Unable to load Articulated Body Description from file \"" << argBodyDescFilename << "\".  Using defaults.\n";
		outBodyDesc.height = DEFAULT_BODY_HEIGHT;
		outBodyDesc.weight = DEFAULT_BODY_WEIGHT;
		outBodyDesc.hipBreadth = DEFAULT_HIP_BREADTH;
	}
}

Else::HumanoidArticulatedBody
::HumanoidArticulatedBody( dWorldID argWorldID, dSpaceID argSpaceID,
                           double argHeight, double argWeight, double argHipBreadth )
:   ArticulatedBody( argWorldID, argSpaceID ),
    myHeight( argHeight ), myWeight( argWeight ),
    myAnkleStrength( DEFAULT_ANKLE_STRENGTH ),
    myFootLengthPerHeight( 0.12 ),
    myFootWidthPerHeight( 0.075 )
{
    myDesc.height = myHeight;
    myDesc.weight = myWeight;
    myDesc.hipBreadth = argHipBreadth;
    createBody();
}

Else::HumanoidArticulatedBody
::~HumanoidArticulatedBody()
{
    clear();
}


void
Else::HumanoidArticulatedBody
::setAnkleStrength( double arg )
{
    myAnkleStrength = arg;
}


void
Else::HumanoidArticulatedBody
::setFootLengthPerHeight( double arg )
{
    myFootLengthPerHeight = arg;
}

void
Else::HumanoidArticulatedBody
::setFootWidthPerHeight( double arg )
{
    myFootWidthPerHeight = arg;
}

void
Else::HumanoidArticulatedBody
::createBodySegments( double argHipWidth )
{
    double ht = myHeight;
    double wt = myWeight;
    
    myBodies.clear();
    // Head
    
    // Trunk
    createVerticalSegment( Trunk,
                          0, 0.708 * ht, .192 * ht, .25 * ht, 39.8  *  wt, .12 * ht );
    
    // Waist 
    createVerticalSegment( Waist, 
                          0, .557 * ht, 0.192 * ht, 0.05 * ht, 7.96  *  wt);
    
    // Thighs
    createVerticalSegment( LeftThigh,
                          //        argHipWidth * ht * -1, 0.407 * ht, 0.09 * ht, .249 * ht, 7.45  *  wt);
                          //        argHipWidth * ht * -1, 0.407 * ht, 0.09 * ht, .255 * ht, 7.45  *  wt);
                          argHipWidth * ht * -1,
                          0.407 * ht,
                          0.07 * ht,
                          .255 * ht,
                          7.45  *  wt);
    
    createVerticalSegment( RightThigh,
                          //        argHipWidth * ht *  1, 0.407 * ht, 0.09 * ht, .249 * ht, 7.45  *  wt);
                          //        argHipWidth * ht * 1, 0.407 * ht, 0.09 * ht, .255 * ht, 7.45  *  wt);
                          argHipWidth * ht * 1, 0.407 * ht, 0.07 * ht, .255 * ht, 7.45  *  wt);
    
    // Shanks
    createVerticalSegment( LeftShank,
                          //       argHipWidth * ht * -1, 0.175 * ht, 0.06 * ht, 0.25 * ht, 3.6 * wt );
                          argHipWidth * ht * -1, /* lateralDist */
                          0.175 * ht, /* center */
                          0.055 * ht, /* width */
                          0.25 * ht,  /* height */
                          3.6 * wt ); /* weight */
    
    createVerticalSegment( RightShank, 
                           //        argHipWidth * ht *  1, 0.175 * ht, 0.06 * ht, 0.25 * ht, 3.6 * wt );
                           argHipWidth * ht *  1, 
                           0.175 * ht, 
                           0.055 * ht, 
                           0.25 * ht, 
                           3.6 * wt );

    double footWidth = myFootWidthPerHeight * ht; //0.07 * ht;
	double footSeparation = 2.0 * argHipWidth * ht;
	double minFootSpacing = 0.02;
	if( footWidth > ( footSeparation - minFootSpacing ) )
	{
		footWidth = footSeparation - minFootSpacing;
	}
    
    //
    // Problem:  changes to foot length must be reflected in 
    // changes to the initial state!
    //
    const double footLength = myFootLengthPerHeight*ht;
    const double footOffset = 0.5*footLength - (0.055*ht);
	createVerticalSegment( RightFoot,
                          argHipWidth * ht * 1, // lateral dist
                          0.03 * ht,            // center 
                          footWidth /*0.085 * ht*/,  // width
                          0.04 * ht,            // height
                          3.6 * wt,             // weight
                          footLength,           // length
                          footOffset );         // length offset
                          
    createVerticalSegment( LeftFoot,
                           argHipWidth * ht * -1,
                           0.03 * ht,
                           footWidth /*0.085 * ht*/,
                           0.04 * ht,
                           3.6 * wt,
                           footLength,
                           footOffset );
    //    dReal* lFPos = dBodyGetPosition( leftFoot );
    //    dBodySetPosition( leftFoot, lFPos[0], lFPos[1]+footLength*2.0, lFPos[2] );
    
    
    /* April 20th version	
     // Feet
     createVerticalSegment( LeftFoot,
     argHipWidth * ht * -1, 0.03 * ht, 0.09 * ht, 0.04 * ht, 3.6 * wt, 0.1 * ht );
     
     createVerticalSegment( RightFoot,
     argHipWidth * ht * 1, 0.03 * ht, 0.09 * ht, 0.04 * ht, 3.6 * wt, 0.1 * ht );
     */
}


void
Else::HumanoidArticulatedBody
::createBody( void )
{
    //std::cerr << "\tArticulatedBody::createBody\n";
    
    using namespace gmtl::Math;
    
    double ht = myHeight;
    double wt = myWeight;
    
	// These limit the max VELOCITY.
	double vel = 75.0;
	myRightHipVelScale[0] = vel; 
	myRightHipVelScale[1] = vel; 
	myRightHipVelScale[2] = vel; 
    
	myLeftHipVelScale[0] =	vel;
	myLeftHipVelScale[1] =	vel;
	myLeftHipVelScale[2] =	vel;
        
    double spineResistanceForce =  2 * wt * myForceScale;
    double spine2ResistanceForce = 2 * wt * myForceScale;
    double spine3ResistanceForce = 2 * wt * myForceScale;
    
    //double hipWidthPerHeight = 0.075; // 0.096;
    double hipWidthPerHeight = ( 0.7 * myDesc.hipBreadth ) / myHeight; //0.075; // 0.096;
    
    createBodySegments( hipWidthPerHeight );
    
    myJointGroupID = 0; //dJointGroupCreate( 1000 );  seems to cause ODE some problems
    
    //std::cerr << "\tArticulatedBody::createBody -- creating torso & spine\n";
    
    dJointID spine = dJointCreateBall( myWorldID, myJointGroupID );
	myJoints[Spine] = spine;
    dJointAttach( spine, myBodies[Trunk], myBodies[Waist] );
    dJointSetBallAnchor( spine, 0, 0, .582 * ht );
    dJointID spineMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
    myMotors[Spine] = spineMotor;
    dJointAttach( spineMotor, myBodies[Trunk], myBodies[Waist] );
    dJointSetAMotorNumAxes( spineMotor, 2 );
    // zeroth axis is y and anchored to the trunk
    dJointSetAMotorAxis( spineMotor, 0, 1, 1, 0, 0 );
    // second axis is z and anchored to the waist
    dJointSetAMotorAxis( spineMotor, 2, 2, 0, 1, 0 );
    // zeroth axis (Y) stops  (shoulder-to-shoulder rotation axis)
    dJointSetAMotorParam( spineMotor, dParamLoStop, PI / -16.0 );
    dJointSetAMotorParam( spineMotor, dParamHiStop, PI / 16. );
    dJointSetAMotorParam( spineMotor, dParamFudgeFactor, 0.01);
    // first axis (X) stops (sit-up axis)
    dJointSetAMotorParam( spineMotor, dParamLoStop2, PI / -16.0 );
    dJointSetAMotorParam( spineMotor, dParamHiStop2, PI / 16.0 );
    dJointSetAMotorParam( spineMotor, dParamFudgeFactor, 0.01);
    // second axis (Z) stops (twist axis)
    dJointSetAMotorParam( spineMotor, dParamLoStop3, PI / -16.0 );
    dJointSetAMotorParam( spineMotor, dParamHiStop3, PI / 16.0 );
    dJointSetAMotorParam( spineMotor, dParamFudgeFactor, 0.01);
    
    dJointSetAMotorMode( spineMotor, dAMotorEuler );
    
    dJointSetAMotorParam( spineMotor, dParamFMax,   spineResistanceForce );  // spine is strong
    dJointSetAMotorParam( spineMotor, dParamBounce, 0.01 );  // 1 is max bounce.  
    dJointSetAMotorParam( spineMotor, dParamVel,    0.0 );  // initial
    dJointSetAMotorParam( spineMotor, dParamFMax2,   spine2ResistanceForce );  // strong
    dJointSetAMotorParam( spineMotor, dParamBounce2, 0.01 );  // 1 is max bounce.
    dJointSetAMotorParam( spineMotor, dParamVel2,    0.0 );  // initial
    dJointSetAMotorParam( spineMotor, dParamFMax3,   spine3ResistanceForce );  // twist is weak
    dJointSetAMotorParam( spineMotor, dParamBounce3, 0.010 );  // 1 is max bounce.  
    dJointSetAMotorParam( spineMotor, dParamVel3,    0.0 );  // initial
    
    //std::cerr << "\tArticulatedBody::createBody -- creating hips\n";
    
    addHip( gmtl::Vec3d( hipWidthPerHeight * ht, 0, .531 * ht ), 
           myBodies[RightThigh], RightHip );
    addHip( gmtl::Vec3d( -1. * hipWidthPerHeight * ht, 0, .531 * ht ), 
           myBodies[LeftThigh], LeftHip );
    //std::cerr << "\tArticulatedBody::createBody -- creating knees\n";
    
    createKnee( gmtl::Vec3d( hipWidthPerHeight * ht *  -1, 0, 0.283 * ht ), 
               myBodies[LeftThigh], 
               myBodies[LeftShank],
               LeftKnee );
    createKnee( gmtl::Vec3d( hipWidthPerHeight * ht * 1, 0, 0.283 * ht ), 
               myBodies[RightThigh], 
               myBodies[RightShank],
               RightKnee);
    
    
    //std::cerr << "\tArticulatedBody::createBody -- creating ankles\n";
    // TIED to createBodySegments() -> createVerticalSegment
    //const double footLength = myFootLengthPerHeight*ht;
    addAnkle( gmtl::Vec3d( hipWidthPerHeight * ht * 1, 
                           0.0f, 
                           0.04f * ht ),
             myBodies[RightShank],
             myBodies[RightFoot],
             RightAnkle );
	addAnkle( gmtl::Vec3d( hipWidthPerHeight * ht * -1, 
                           0.0f, 
                           0.04f * ht ),
             myBodies[LeftShank],
             myBodies[LeftFoot],
             LeftAnkle );
    
    /*
     //ANKLE JOINTS
     dJointID ankle = dJointCreateBall( myWorldID, myJointGroupID );
     dJointAttach( ankle, foot, shank );
     dJointSetBallAnchor( ankle, latDist, 0, 0.04f * ht );
     dJointID ankleMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
     myJoints.push_back( ankleMotor );
     myAnkleJoint[i == -1] = ankleMotor;
     dJointAttach( ankleMotor, foot, shank );
     dJointSetAMotorMode( ankleMotor, dAMotorEuler );
     // zeroth axis is y and anchored to the foot
     dJointSetAMotorAxis( ankleMotor, 0, 1, 0, 1, 0 );
     // second axis is z and anchored to the shank
     dJointSetAMotorAxis( ankleMotor, 2, 2, 0, 0, 1 );
     // zeroth axis (Y) stops  (heel-to-toe axis)
     dJointSetAMotorParam( ankleMotor, dParamLoStop, PI / -8.0 );
     dJointSetAMotorParam( ankleMotor, dParamHiStop, PI / 8.0 );
     dJointSetAMotorParam( ankleMotor, dParamFMax,   ankleResistanceForce );  // twist is weak
     dJointSetAMotorParam( ankleMotor, dParamBounce, 0.10 );  // 1 is max bounce.  
     dJointSetAMotorParam( ankleMotor, dParamVel,    0.0 );  // initial
     // first axis (X) stops (achilles-tendon axis)
     dJointSetAMotorParam( ankleMotor, dParamLoStop2, PI / -2.0 );
     dJointSetAMotorParam( ankleMotor, dParamHiStop2, PI / 12.0 );
     dJointSetAMotorParam( ankleMotor, dParamFMax2,   ankle2ResistanceForce );  // strong
     dJointSetAMotorParam( ankleMotor, dParamBounce2, 0.1 );  // 1 is max bounce.  bouncy
     dJointSetAMotorParam( ankleMotor, dParamVel2,    0.0 );  // initial
     // second axis (Z) stops (right-to-left axis)
     dJointSetAMotorParam( ankleMotor, dParamLoStop3, PI / -4.0 );
     dJointSetAMotorParam( ankleMotor, dParamHiStop3, PI / 4.0 );
     dJointSetAMotorParam( ankleMotor, dParamFMax3,   ankle3ResistanceForce );  // flick is weak
     dJointSetAMotorParam( ankleMotor, dParamBounce3, 0.10 );  // 1 is max bounce.  
     dJointSetAMotorParam( ankleMotor, dParamVel3,    0.0 );  // initial
     
     */
    
    /*
     dBodyID trunk = createVerticalSegment(
     0, .708 * ht, .192 * ht, .25 * ht, 39.8  *  wt );
     myTrunkBodyID = trunk;
     dJointID spine = dJointCreateBall( myWorldID, myJointGroupID );
     dJointAttach( spine, trunk, waist );
     dJointSetBallAnchor( spine, 0, 0, .582 * ht );
     dJointID spineMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
     myJoints.push_back( spineMotor );
     dJointAttach( spineMotor, trunk, waist );
     dJointSetAMotorMode( spineMotor, dAMotorEuler );
     // zeroth axis is y and anchored to the trunk
     dJointSetAMotorAxis( spineMotor, 0, 1, 0, 1, 0 );
     // second axis is z and anchored to the waist
     dJointSetAMotorAxis( spineMotor, 2, 2, 0, 0, 1 );
     // zeroth axis (Y) stops  (shoulder-to-shoulder rotation axis)
     dJointSetAMotorParam( spineMotor, dParamLoStop, PI / -8.0 );
     dJointSetAMotorParam( spineMotor, dParamHiStop, PI / 8. );
     dJointSetAMotorParam( spineMotor, dParamFMax,   spineResistanceForce );  // spine is strong
     dJointSetAMotorParam( spineMotor, dParamBounce, 0.01 );  // 1 is max bounce.  
     dJointSetAMotorParam( spineMotor, dParamVel,    0.0 );  // initial
     // first axis (X) stops (sit-up axis)
     dJointSetAMotorParam( spineMotor, dParamLoStop2, PI / -10.0 );
     dJointSetAMotorParam( spineMotor, dParamHiStop2, PI / 10.0 );
     dJointSetAMotorParam( spineMotor, dParamFMax2,   spine2ResistanceForce );  // strong
     dJointSetAMotorParam( spineMotor, dParamBounce2, 0.01 );  // 1 is max bounce.
     dJointSetAMotorParam( spineMotor, dParamVel2,    0.0 );  // initial
     // second axis (Z) stops (twist axis)
     dJointSetAMotorParam( spineMotor, dParamLoStop3, PI / -6.0 );
     dJointSetAMotorParam( spineMotor, dParamHiStop3, PI / 6.0 );
     dJointSetAMotorParam( spineMotor, dParamFMax3,   spine3ResistanceForce );  // twist is weak
     dJointSetAMotorParam( spineMotor, dParamBounce3, 0.010 );  // 1 is max bounce.  
     dJointSetAMotorParam( spineMotor, dParamVel3,    0.0 );  // initial
     */
    /*
     dBodyID head = createVerticalSegment(
     0, .917 * ht, .1 * ht, .1 * ht, 5.0  *  wt );
     myHeadBodyID = head;
     dJointID neck = dJointCreateBall( myWorldID, myJointGroupID );
     dJointAttach( neck, trunk, head );
     dJointSetBallAnchor( neck, 0, 0, .833 * ht );
     dJointID neckMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
     myJoints.push_back( neckMotor );
     dJointAttach( neckMotor, trunk, head );
     dJointSetAMotorMode( neckMotor, dAMotorEuler );
     // zeroth axis is y and anchored to the head
     dJointSetAMotorAxis( neckMotor, 0, 1, 0, 1, 0 );
     // second axis is z and anchored to the trunk
     dJointSetAMotorAxis( neckMotor, 2, 2, 0, 0, 1 );
     // zeroth axis (Y) stops  (ear-to-ear rotation axis)
     dJointSetAMotorParam( neckMotor, dParamLoStop, PI / -6.0 );
     dJointSetAMotorParam( neckMotor, dParamHiStop, PI / 6.0 );
     dJointSetAMotorParam( neckMotor, dParamFMax,   neckResistanceForce );  // 
     dJointSetAMotorParam( neckMotor, dParamBounce, 0.015 );  // 1 is max bounce.  
     dJointSetAMotorParam( neckMotor, dParamVel,    0.0 );  // initial
     // first axis (X) stops (head-butt axis)
     dJointSetAMotorParam( neckMotor, dParamLoStop2, PI / -6.0 );
     dJointSetAMotorParam( neckMotor, dParamHiStop2, PI / 2.0 );
     dJointSetAMotorParam( neckMotor, dParamFMax2,   neck2ResistanceForce );  // strong
     dJointSetAMotorParam( neckMotor, dParamBounce2, 0.03 );  // 1 is max bounce.
     dJointSetAMotorParam( neckMotor, dParamVel2,    0.0 );  // initial
     // second axis (Z) stops (twist axis)
     dJointSetAMotorParam( neckMotor, dParamLoStop3, PI / -3.0 );
     dJointSetAMotorParam( neckMotor, dParamHiStop3, PI / 3.0 );
     dJointSetAMotorParam( neckMotor, dParamFMax3,   neck3ResistanceForce );  // twist is weak
     dJointSetAMotorParam( neckMotor, dParamBounce3, 0.010 );  // 1 is max bounce.  
     dJointSetAMotorParam( neckMotor, dParamVel3,    0.0 );  // initial
     */
    
    //std::cerr << "\tArticulatedBody::createBody -- done.\n";
    
}


void
Else::HumanoidArticulatedBody
::getSensors( ControllerPtr argController, double argDeltaTime )
{

    // Sensors
    //  5 shared
    //  6 per side
    //  0 no extra bias
    //  1 crossover

    myLastTimeStep = argDeltaTime;
    const dReal* leftFootPos = dBodyGetPosition( myBodies[LeftFoot] );
    const dReal* rightFootPos = dBodyGetPosition( myBodies[RightFoot] );
    const dReal* com = dBodyGetPosition( myBodies[Waist] );
    
    // velocity in plane
    const dReal* av = dBodyGetLinearVel( myBodies[Waist] );
    argController->setInput( 0, 0, av[0] );
    argController->setInput( 0, 1, av[1] );
    // get Z-component of Torso's position
    argController->setInput( 0, 2, com[2] );
    
    // Still both-- tag for swing foot (aka, tactile sensors on feet
    SideEnum sideCol = getSideOfLastCollision();
    /// 
    argController->setInput( 0, 3, (sideCol == LeftSide) ? 1.0 : 0.0 );
    argController->setInput( 0, 4, (sideCol == RightSide) ? 1.0 : 0.0 );
    
    // RIGHT
    const int rightSide = 1;
    argController->setInput( rightSide, 0, rightFootPos[0] - com[0] );
    argController->setInput( rightSide, 1, rightFootPos[1] - com[1] );

    argController->setInput( rightSide, 2, dJointGetAMotorAngle( myMotors[RightAnkle], 2) );
    argController->setInput( rightSide, 3, dJointGetHingeAngle( myJoints[RightKnee] ) );
    argController->setInput( rightSide, 4, dJointGetAMotorAngle( myMotors[RightHip], 2) );
    argController->setInput( rightSide, 5, dJointGetAMotorAngle( myMotors[RightHip], 0) );

//    outSensors.push_back( rightFootPos[0] - com[0] );
//    outSensors.push_back( rightFootPos[1] - com[1] );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[RightAnkle], 2) );
//    outSensors.push_back( dJointGetHingeAngle( myJoints[RightKnee] ) );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[RightHip], 2) );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[RightHip], 0) );
    // 5 + 6 = 11
    
    // LEFT
    const int leftSide = 2;
    argController->setInput( leftSide, 0, com[0] - leftFootPos[0] );
    argController->setInput( leftSide, 1, leftFootPos[1] - com[1] );

    argController->setInput( leftSide, 2, dJointGetAMotorAngle( myMotors[LeftAnkle], 2) );
    argController->setInput( leftSide, 3, dJointGetHingeAngle( myJoints[LeftKnee] ) );
    argController->setInput( leftSide, 4, dJointGetAMotorAngle( myMotors[LeftHip], 2) );
    argController->setInput( leftSide, 5, dJointGetAMotorAngle( myMotors[LeftHip], 0) );

//    outSensors.push_back( com[0] - leftFootPos[0] );
//    outSensors.push_back( leftFootPos[1] - com[1] );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftAnkle], 2) );
//    outSensors.push_back( dJointGetHingeAngle( myJoints[LeftKnee] ) );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftHip], 2) );
//    outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftHip], 0) );
    // 5 + 6 = 11
    
    // Or, 17 all together
}
//static int getSensorCount( void )
//{
//    return (2 * (getSensorCountBoth())) + getSensorCountExtraEachSide() + ;
//}
/*
 /// Fill the provided vector with current sensor data.
 void getSensors( std::vector< double >& outSensors, double argDeltaTime )
 {
 myLastTimeStep = argDeltaTime;
 outSensors.clear();
 
 // BOTH
 // 0
 const dReal* av = dBodyGetAngularVel( myBodies[Waist] );
 for( int i = 0; i < 3; ++i )
 {
 outSensors.push_back( av[i] );
 }
 // 3
 const dReal* pos = dBodyGetPosition( myBodies[Waist] );
 
 // get Z-component of Torso
 outSensors.push_back( pos[2] );
 // 4
 
 // RIGHT
 outSensors.push_back( dJointGetAMotorAngle( myMotors[RightAnkle], 2) );
 outSensors.push_back( dJointGetHingeAngle( myJoints[RightKnee] ) );
 outSensors.push_back( dJointGetAMotorAngle( myMotors[RightHip], 2) );
 outSensors.push_back( dJointGetAMotorAngle( myMotors[RightHip], 0) );
 // 4 + 4 = 8
 
 // LEFT
 outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftAnkle], 2) );
 outSensors.push_back( dJointGetHingeAngle( myJoints[LeftKnee] ) );
 outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftHip], 2) );
 outSensors.push_back( dJointGetAMotorAngle( myMotors[LeftHip], 0) );
 // 4 + 4 = 8
 
 //outSensors.push_back( sin( myHeartbeat[0] ) );
 //outSensors.push_back( cos( myHeartbeat[1] ) );
 }
 
 static int getInputCountPerNetwork( void )
 {
 return 8;
 }
 static int getSensorCount( void )
 {
 return 12;
 }
 */


void
Else::HumanoidArticulatedBody
::setActuators( const ControllerPtr argController )
{
    // Actuators
    //  1 shared
    //  3 module
    //  1 crossover


    //////////////////////////////////////////////////////////////////////////
    // Apply Controlled Actuators
    //////////////////////////////////////////////////////////////////////////
    
    // RIGHT
    const int right = 1;
    int rightIdx = 0;
    if( CONTROL_KNEE_ACTUATOR )
    {
        actuateHingePos( myJoints[RightKnee], 
                         argController->getOutput( right, rightIdx++ ) );
    }
    maintainAMotorAngle( myMotors[RightHip], 0, 
                         argController->getOutput( right, rightIdx++ ) );
    maintainAMotorAngle( myMotors[RightHip], 2, 
                         argController->getOutput( right, rightIdx++ ) );
    
    // LEFT
    const int left = 2;
    int leftIdx = 0;
    if( CONTROL_KNEE_ACTUATOR )
    {
        actuateHingePos( myJoints[LeftKnee], 
                         argController->getOutput( left, leftIdx++ ) );
    }
    maintainAMotorAngle( myMotors[LeftHip], 0,
                         argController->getOutput( left, leftIdx++ ) );
    maintainAMotorAngle( myMotors[LeftHip], 2, 
                         argController->getOutput( left, leftIdx++ ) );
    
    // Center
    if( CONTROL_SPINE_ACTUATOR )
    {
        maintainAMotorAngle( myMotors[Spine], 2, 
                             argController->getOutput( 0, 0 ) );
    }
    
    //////////////////////////////////////////////////////////////////////////
    // Update Passive Actuators
    //////////////////////////////////////////////////////////////////////////
    
    maintainAMotorAngle( myMotors[RightHip], 1, .5 );
    maintainAMotorAngle( myMotors[LeftHip],  1, .5 );
    
    maintainAMotorAngle( myMotors[Spine], 0, .5 );
    maintainAMotorAngle( myMotors[Spine], 1, .5 );
    
    // NOTE:  If Knee is passive it left to swing loose-- no target position!
    
    if( !CONTROL_SPINE_ACTUATOR )
    {
        maintainAMotorAngle( myMotors[Spine], 2, .5 );
    }
    
    maintainAMotorAngle( myMotors[RightAnkle], 0, 0.5 );
    maintainAMotorAngle( myMotors[RightAnkle], 1, 0.5 );
    maintainAMotorAngle( myMotors[RightAnkle], 2, 0.5 );
    
    maintainAMotorAngle( myMotors[LeftAnkle], 0, 0.5 );
    maintainAMotorAngle( myMotors[LeftAnkle], 1, 0.5 );
    maintainAMotorAngle( myMotors[LeftAnkle], 2, 0.5 );
}



void 
Else::HumanoidArticulatedBody
::writeCurrJointTraces( double argT )
{
    if( myJointTraceFile )
    {
        myJointTraceFile 
        << argT << '\t'
        << dJointGetAMotorAngle( myMotors[RightHip], 0 ) << '\t'
        << dJointGetAMotorAngle( myMotors[RightHip], 2 ) << '\t'
        << dJointGetHingeAngle( myJoints[RightKnee] ) << '\t'
        << dJointGetAMotorAngle( myMotors[RightAnkle], 0 ) << '\t'
        << dJointGetAMotorAngle( myMotors[RightAnkle], 2 ) << '\t'
        
        << dJointGetAMotorAngle( myMotors[LeftHip], 0 ) << '\t'
        << dJointGetAMotorAngle( myMotors[LeftHip], 2 ) << '\t'
        << dJointGetHingeAngle( myJoints[LeftKnee] ) << '\t'
        << dJointGetAMotorAngle( myMotors[LeftAnkle], 0 ) << '\t'
        << dJointGetAMotorAngle( myMotors[LeftAnkle], 2 ) << '\t'
        
        << dBodyGetPosition( myBodies[Waist] )[0] << '\t'
        << dBodyGetPosition( myBodies[Waist] )[1] << '\t'
        << dBodyGetPosition( myBodies[Waist] )[2] << '\t'
        
        << dBodyGetQuaternion( myBodies[Waist] )[0] << '\t'
        << dBodyGetQuaternion( myBodies[Waist] )[1] << '\t'
        << dBodyGetQuaternion( myBodies[Waist] )[2] << '\t'
        << dBodyGetQuaternion( myBodies[Waist] )[3] << '\n';
    }
}


void 
Else::HumanoidArticulatedBody
::acquireJointAngleData( std::vector< double >& out )
{
    out.push_back(  dJointGetAMotorAngle( myMotors[RightHip], 0 ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[RightHip], 2 ) );
    out.push_back(  dJointGetHingeAngle( myJoints[RightKnee] ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[RightAnkle], 0 ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[RightAnkle], 2 ) );
    
    out.push_back(  dJointGetAMotorAngle( myMotors[LeftHip], 0 ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[LeftHip], 2 ) );
    out.push_back(  dJointGetHingeAngle( myJoints[LeftKnee] ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[LeftAnkle], 0 ) );
    out.push_back(  dJointGetAMotorAngle( myMotors[LeftAnkle], 2 ) );
    
    out.push_back(  dBodyGetPosition( myBodies[Waist] )[0] );
    out.push_back(  dBodyGetPosition( myBodies[Waist] )[1] );
    out.push_back(  dBodyGetPosition( myBodies[Waist] )[2] );
    
    out.push_back(  dBodyGetQuaternion( myBodies[Waist] )[0] );
    out.push_back(  dBodyGetQuaternion( myBodies[Waist] )[1] );
    out.push_back(  dBodyGetQuaternion( myBodies[Waist] )[2] );
    out.push_back(  dBodyGetQuaternion( myBodies[Waist] )[3] );
}


double
Else::HumanoidArticulatedBody
::getInitialWaistHeight( void )
{
    return (myHeight * 0.657);  // fixed for current morphology!
}

double 
Else::HumanoidArticulatedBody
::getWaistHeight( void )
{
    return dBodyGetPosition( myBodies[Waist] )[2];
}

double
Else::HumanoidArticulatedBody
::getWeight( void )
{
    return myWeight;
}

void
Else::HumanoidArticulatedBody
::addHip( gmtl::Vec3d argAnchor, dBodyID argThigh, BodyJointEnum argJointName )
{
    assert( myBodies[Waist] != (dBodyID)0 );
    assert( argThigh != (dBodyID)0 );
    assert( argThigh != myBodies[Waist] );
    
    using namespace gmtl::Math;
    
    double wt = myWeight;
    
    double hipForce	= HIP_TORQUE_SCALE * wt * myForceScale;
    double hip2Force	= HIP_TORQUE_SCALE * wt * myForceScale;
    double hip3Force	= HIP_TORQUE_SCALE * wt * myForceScale;
    
    dJointID hipJoint = dJointCreateBall( myWorldID, myJointGroupID );
    myJoints[argJointName] = hipJoint;
    dJointAttach( hipJoint,  myBodies[Waist],  argThigh );
    dJointSetBallAnchor( hipJoint, argAnchor[0], argAnchor[1], argAnchor[2] );
    dJointID hipMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
    myMotors[argJointName] = hipMotor;
    
    dJointAttach( hipMotor, myBodies[Waist], argThigh );
    dJointSetAMotorNumAxes( hipMotor, 2 );
    // zeroth axis is y and anchored to the thigh
    dJointSetAMotorAxis( hipMotor, 0, 1, 1, 0, 0 );
    // second axis is z and anchored to the waist
    dJointSetAMotorAxis( hipMotor, 2, 2, 0, 1, 0 );
	// ? axis (X) stops (front-kicking axis)
    dJointSetAMotorParam( hipMotor, dParamLoStop,        PI / -2 );
    dJointSetAMotorParam( hipMotor, dParamHiStop,        PI / 6 );
    dJointSetAMotorParam( hipMotor, dParamFudgeFactor,   0.0 );
    
    dJointSetAMotorParam( hipMotor, dParamLoStop2,       PI / -6 );
    dJointSetAMotorParam( hipMotor, dParamHiStop2,       PI / 6.0 );
    dJointSetAMotorParam( hipMotor, dParamFudgeFactor2,  0.0 );
    // ? axis (Y) stops  (sweeping right-to-left axis)
    dJointSetAMotorParam( hipMotor, dParamLoStop3,       PI / -12.0 );
    dJointSetAMotorParam( hipMotor, dParamHiStop3,       PI / 4.0 );
    dJointSetAMotorParam( hipMotor, dParamFudgeFactor3,  0.0 );
    
    dJointSetAMotorMode( hipMotor, dAMotorEuler );
    
    dJointSetAMotorParam( hipMotor, dParamFMax,   hipForce );  // sweep is weak
    dJointSetAMotorParam( hipMotor, dParamBounce, 0.0 );  // 1 is max bounce.  
    dJointSetAMotorParam( hipMotor, dParamVel,    0.0 );  // initial
    dJointSetAMotorParam( hipMotor, dParamFMax2,   hip2Force );  // strong
    dJointSetAMotorParam( hipMotor, dParamBounce2, 0.0 );  // 1 is max bounce.
    dJointSetAMotorParam( hipMotor, dParamVel2,    0.0 );  // initial
    dJointSetAMotorParam( hipMotor, dParamFMax3,   hip3Force );  // flick is weak
    dJointSetAMotorParam( hipMotor, dParamBounce3, 0.0 );  // 1 is max bounce.  
    dJointSetAMotorParam( hipMotor, dParamVel3,    0.0 );  // initial
}


void 
Else::HumanoidArticulatedBody
::createKnee( gmtl::Vec3d argAnchor, dBodyID argThigh, dBodyID argShank, BodyJointEnum argJointName )
{
    using namespace gmtl::Math;
    
    // Standard -- 0.1 factor
    //double kneeForce = 0.1 * myWeight * myForceScale;
    
    
    double kneeForce = KNEE_TORQUE_SCALE * myWeight * myForceScale;
    
    dJointID knee = dJointCreateHinge( myWorldID, myJointGroupID );
    myJoints[argJointName] = knee;
    
    // ORDER
    dJointAttach( knee, argShank, argThigh );
    dJointSetHingeAnchor( knee, argAnchor[0], argAnchor[1], argAnchor[2] );
    dJointSetHingeAxis( knee, 1.0, 0.0, 0.0 );
    
    
    // Only if using ODE's joint limits:  
    //dJointSetHingeParam( knee, dParamHiStop, PI / 24.);  
    dJointSetHingeParam( knee, dParamHiStop, 0.01 );
    
    dJointSetHingeParam( knee, dParamLoStop, -2.*PI / 3.0);
    
    dJointSetHingeParam( knee, dParamFMax,   kneeForce );
    dJointSetHingeParam( knee, dParamBounce, 0.0 );
    dJointSetHingeParam( knee, dParamVel,    0);
}

void
Else::HumanoidArticulatedBody
::addAnkle( gmtl::Vec3d argAnchor, dBodyID argShank, dBodyID argFoot, BodyJointEnum argJointName )
{
	assert( argShank != (dBodyID)0 );
	assert( argFoot != (dBodyID)0 );
	assert( argFoot != argShank );
    
	using namespace gmtl::Math;
    
	double wt = myWeight;
    
    double ankleForce	= myAnkleStrength * wt * myForceScale;
    double ankle2Force	= myAnkleStrength * wt * myForceScale;
    double ankle3Force	= myAnkleStrength * wt * myForceScale;
    
	dJointID ankleJoint = dJointCreateBall( myWorldID, myJointGroupID );
	myJoints[argJointName] = ankleJoint;
	dJointAttach( ankleJoint,  argShank,  argFoot );
	dJointSetBallAnchor( ankleJoint, argAnchor[0], argAnchor[1], argAnchor[2] );
	dJointID ankleMotor = dJointCreateAMotor( myWorldID, myJointGroupID );
	myMotors[argJointName] = ankleMotor;

    
    //    dJointAttach( ankleMotor, foot, shank );
    //dJointSetAMotorMode( ankleMotor, dAMotorEuler );
    //// zeroth axis is y and anchored to the foot
    //dJointSetAMotorAxis( ankleMotor, 0, 1, 0, 1, 0 );
    //// second axis is z and anchored to the shank
    //dJointSetAMotorAxis( ankleMotor, 2, 2, 0, 0, 1 );
    //// zeroth axis (Y) stops  (heel-to-toe axis)
    
	dJointAttach( ankleMotor, argShank, argFoot );
	dJointSetAMotorNumAxes( ankleMotor, 2 );
	// zeroth axis is y and anchored to the thigh
	//  -- dJointSetAMotorAxis( ankleMotor, 0, 1, -1, 0, 0 );
	dJointSetAMotorAxis( ankleMotor, 0, 1, 0, 1, 0 );
    // second axis is z and anchored to the waist
	//dJointSetAMotorAxis( ankleMotor, 2, 2, 0, 1, 0 );
	dJointSetAMotorAxis( ankleMotor, 2, 2, 0, 0, 1 );
    // ? axis (X) stops (front-kicking axis)
    
    
    // roll (Brings big tow above little toe)
    dJointSetAMotorParam( ankleMotor, dParamLoStop,        2*PI / -3 );
    dJointSetAMotorParam( ankleMotor, dParamHiStop,        2*PI / 3  );
	dJointSetAMotorParam( ankleMotor, dParamFudgeFactor,   0.01 );
    
    dJointSetAMotorParam( ankleMotor, dParamLoStop2,       PI / -6 );
    dJointSetAMotorParam( ankleMotor, dParamHiStop2,       PI / 6 );
	dJointSetAMotorParam( ankleMotor, dParamFudgeFactor2,  0.01 );
	// ? axis (Y) stops  (sweeping right-to-left axis)
	dJointSetAMotorParam( ankleMotor, dParamLoStop3,       PI / -6 );
	dJointSetAMotorParam( ankleMotor, dParamHiStop3,       PI / 6 );
	dJointSetAMotorParam( ankleMotor, dParamFudgeFactor3,  0.01 );
    
	dJointSetAMotorMode( ankleMotor, dAMotorEuler );
    
	dJointSetAMotorParam( ankleMotor, dParamFMax,   ankleForce );  // sweep is weak
	dJointSetAMotorParam( ankleMotor, dParamBounce, 0.0 );  // 1 is max bounce.  
	dJointSetAMotorParam( ankleMotor, dParamVel,    0.0 );  // initial
	dJointSetAMotorParam( ankleMotor, dParamFMax2,   ankle2Force );  // strong
	dJointSetAMotorParam( ankleMotor, dParamBounce2, 0.0 );  // 1 is max bounce.
	dJointSetAMotorParam( ankleMotor, dParamVel2,    0.0 );  // initial
	dJointSetAMotorParam( ankleMotor, dParamFMax3,   ankle3Force );  // flick is weak
	dJointSetAMotorParam( ankleMotor, dParamBounce3, 0.0 );  // 1 is max bounce.  
	dJointSetAMotorParam( ankleMotor, dParamVel3,    0.0 );  // initial



}



