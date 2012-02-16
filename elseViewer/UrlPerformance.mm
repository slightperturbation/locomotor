/*
 *  UrlPerformance.cpp
 *  elseViewer
 *
 *  Created by vector on 9/7/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "UrlPerformance.h"
#import "UrlPerformanceImpl.h"
#import <Cocoa/Cocoa.h>

#include <sstream>
#include <iostream>

void getPerformanceStates( std::vector< std::vector< Else::BodyState > >& outStates, 
                           const long int argPerformanceId )
{
    UrlPerformanceImpl* perf = [[UrlPerformanceImpl alloc] initWithId: argPerformanceId];
    
    const int size = [perf length] + 1;
    char* buf = new char[ size ];
    [perf getCString: buf maxLength:size];
    
    //std::cerr << "Got Performance data from middle-layer as:\n" << buf << "\n";
    
    std::string str( buf );
    std::stringstream s;
    s.str( str );
    s >> outStates;
    
    std::cerr << "Parsed out " << outStates.size() << " frames from performance_id=" << argPerformanceId << ".\n";

    [perf release];
}

double getPerformanceTimeStep( void )
{
    return 0.0165; //   ----------- TODO!!!!!
}

