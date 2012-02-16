/*
 *  Performance.h
 *  elseViewer
 *
 *  Created by vector on 9/7/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#ifndef ELSE_VIEWER_PERFORMANCE_H
#define ELSE_VIEWER_PERFORMANCE_H

#include <vector>

#include <BodyState.hpp>

/// Abstract superclass for a collection of body states making up an animation.
class Performance
{
public:
    virtual ~Performance() 
    {
        // Noop
    }
    virtual void load( const long int& argPerformanceId ) = 0;
    virtual std::vector< std::vector< Else::BodyState > >& getStates( void ) = 0;
    virtual double getTimeStep( void ) = 0;
};

#endif
