/*
 *  BodyState.cpp
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "BodyState.hpp"

using namespace Else;


std::ostream& operator << ( std::ostream& out, const BodyState& bs )
{
    out << bs.id << ' ';
    for( size_t i=0; i<3; ++i )
    {
        out << bs.pos[i] << ' ';
    }
    for( size_t i=0; i<4; ++i )
    {
        out << bs.rot[i] << ' ';
    }
    for( size_t i=0; i<3; ++i )
    {
        out << bs.vel[i] << ' ';
    }
    for( size_t i=0; i<3; ++i )
    {
        out << bs.angvel[i];
        if( i != 2 ) out << ' ';
    }
    return out;
}

std::ostream& operator << ( std::ostream& out, const std::vector< BodyState >& arg )
{
    out << arg.size() << std::endl;
    for( std::vector<BodyState>::const_iterator iter = arg.begin(); iter != arg.end(); ++iter )
    {
        const BodyState& bs = *iter;
        out << bs << std::endl;
    }
    return out;
}

std::ostream& operator << ( std::ostream& out, const std::vector< std::vector< BodyState > >& anim )
{
    size_t frameCount = anim.size();
    out << frameCount << std::endl;
    for( size_t i = 0; i < frameCount; ++i )
    {
        out << anim[i] << std::endl;
    }
    return out;
}

std::istream& operator >> ( std::istream& in, BodyState& bs )
{
    int bodyEnum;
    in >> bodyEnum;
    if( bodyEnum < 0 )
    {
        std::cerr << "Error parsing BodyState\n";
    }
    bs.id = (BodySegmentEnum) bodyEnum;
    for( size_t i=0; i<3; ++i )
    {
        in >> bs.pos[i];
    }
    for( size_t i=0; i<4; ++i )
    {
        in >> bs.rot[i];
    }
    for( size_t i=0; i<3; ++i )
    {
        in >> bs.vel[i];
    }
    for( size_t i=0; i<3; ++i )
    {
        in >> bs.angvel[i];
    }
    return in;    
}

// load one "frame" of animation, ie, all the body segments in one state
std::istream& operator >> ( std::istream& in, std::vector< BodyState >& arg )
{
    size_t size = 0;
    in >> size;
    for( size_t i = 0; i < size; ++i )
    {
        BodyState bs;
        in >> bs;
        arg.push_back( bs );
    }
    return in;
}

std::istream& operator >> ( std::istream& in, std::vector< std::vector< BodyState > >& arg )
{
    size_t frameCount = 0;
    in >> frameCount;
    arg.clear();
    arg.resize( frameCount );
    for( size_t i = 0; i < frameCount; ++i )
    {
        in >> arg[i]; // loads a vector<BodyState> for frame i
    }
    return in;
}


