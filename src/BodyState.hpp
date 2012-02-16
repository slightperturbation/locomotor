#ifndef BODY_STATE_H
#define BODY_STATE_H
/*
 *  BodyState.h
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright (C) 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <vector>
#include <iostream>

namespace Else
{
    enum MotorTypeEnum		{ MotorPos, MotorVel, MotorTorque };
    
    /// Enums providing a semantic meaning for body parts.  Some characters will 
    /// not use any of these, but the semantics all application (or even partial
    /// application) of various fitness functions and controllers to various
    /// character models (subclasses of ArticulatedBody)
    /// Any segment that will be referenced by the fitness function or other methods
    /// will need an enum here to give it a handle.
    enum BodySegmentEnum	{ Trunk, Waist, LeftThigh, RightThigh, LeftShank, RightShank, LeftFoot, RightFoot };
    enum BodyJointEnum		{ Spine, LeftHip, RightHip, LeftKnee, RightKnee, LeftAnkle, RightAnkle };
    enum SideEnum			{ NeitherSide, RightSide, LeftSide };
    
    struct BodyState
    {
        BodySegmentEnum id;
        double          pos[3];
        double          rot[4];  // (w, x, y, z)
        double          vel[3];
        double          angvel[3];
    };
}    

std::ostream& operator << ( std::ostream& out, const Else::BodyState& bs );
std::ostream& operator << ( std::ostream& out, const std::vector< Else::BodyState >& arg );
std::ostream& operator << ( std::ostream& out, const std::vector< std::vector< Else::BodyState > >& anim );

std::istream& operator >> ( std::istream& in, Else::BodyState& bs );
std::istream& operator >> ( std::istream& in, std::vector< Else::BodyState >& arg );
std::istream& operator >> ( std::istream& in, std::vector< std::vector< Else::BodyState > >& arg );


#endif
