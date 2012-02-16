/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// Copyright (c) 2004, Brian F. Allen, 
//
// Contact: vector@acm.org
//

// Project Includes
#include <Tardis.hpp>

// External Includes
#ifdef WIN32
#include <WINBASE.H>
#include <MMSYSTEM.H>
#else
#include <mach/mach_time.h>
#endif



void
Else::Tardis
::setupTimer( void )
{
#ifdef WIN32
	LARGE_INTEGER rawFrequency;
	// is there a performance counter available?
	if (QueryPerformanceFrequency( &rawFrequency ) ) {
		myHasPerformanceTimer = true;
		myClockRate = (long int) rawFrequency.QuadPart;
	} else {
		myHasPerformanceTimer = false;
		// no performance counter, read in using timeGetTime which uses milliseconds
		myClockRate = 1e-3;
	}	
#else
    mach_timebase_info_data_t time_info;
    mach_timebase_info( &time_info );
    myClockRate = 1e-9 * time_info.numer / (double)time_info.denom;
#endif
	
}

double
Else::Tardis
::getSystemTimerSeconds( void )
{
#ifdef WIN32
	LARGE_INTEGER rawTime;
	if( myHasPerformanceTimer )
	{
		QueryPerformanceCounter(&rawTime);
	} else {
		// no performance counter, read in using timeGetTime
		rawTime.QuadPart = timeGetTime();
	}
	return (double)(rawTime.QuadPart) * (double)myClockRate;
#else
  //timeval tim;
  //gettimeofday( &tim, NULL );
  //double t1 = tim.tv_sec + ( tim.tv_usec / 1000000.0 );
    return mach_absolute_time() * myClockRate;
#endif
  return 0;
}

Else::Tardis
::Tardis( void )
: myTime( 0 ),
	myRate( 1 ),
	myLastElapsedTime( 0 )
{
	setupTimer();
	myLastTimestamp = getSystemTimerSeconds();
}

Else::Time 
Else::Tardis
::getLastDeltaTime( void )
{
	return myLastElapsedTime;
}

double
Else::Tardis
::getFraction( Else::Time argStartTime, Else::Time argEndTime )
{
	return getTime() - argStartTime / ( argEndTime - argStartTime );
}

Else::Time
Else::Tardis
::update( void )
{
	double currTimestamp = getSystemTimerSeconds();
	myLastElapsedTime = myRate * (currTimestamp - myLastTimestamp);
	myTime += myLastElapsedTime;
	myLastTimestamp = currTimestamp;
	return myLastElapsedTime;
}

Else::Time
Else::Tardis
::getTime( void )
{
	return myTime;
}

void
Else::Tardis
::setRate( Else::Time argNewRate )
{
	myRate = argNewRate;
}

void
Else::Tardis
::setTime( Else::Time argNewTime )
{
	myTime = argNewTime;
}


Else::CumulativeTimer
::CumulativeTimer( void )
: myTime( 0 )
{
}

void
Else::CumulativeTimer
::start( void )
{
	myTardis.setTime( 0 );
}

void
Else::CumulativeTimer
::stop( void )
{
	myTime += myTardis.update();
}

void
Else::CumulativeTimer
::reset( void )
{
	myTime = 0;
}

Else::Time
Else::CumulativeTimer
::get( void )
{
	return myTime;
}
