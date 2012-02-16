/*
 *  UrlPerformance.h
 *  elseViewer
 * 
 *  Provides a C bridge between Obj-C and C++ classes
 *  Includeable by C++ classes
 * 
 *  Created by vector on 9/7/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#ifndef ELSE_VIEWER_URL_PERFORMANCE_H
#define ELSE_VIEWER_URL_PERFORMANCE_H

#include <vector>
#include <BodyState.hpp>

void   getPerformanceStates( std::vector< std::vector< Else::BodyState > >& outStates, 
                             const long int argPerformanceId );
double getPerformanceTimeStep( void );

#endif
