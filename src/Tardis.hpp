/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_TARDIS_HPP
#define ELSE_TARDIS_HPP

// Copyright (c) 2004, Brian F. Allen, 
//
// Contact: vector@acm.org
//

/** \file 
    \author Brian F. Allen (vector@acm.org)
*/

// Project Includes
#include <Else.hpp>

// External Includes


namespace Else
{

	/// Time Server.
  /// Maintains the current game time, responsible for time scaling.
  class Tardis
  {
  public:
		Tardis( void );

		/// Gets the current time in seconds.
		Time getTime( void );

		/// Returns the number of seconds elapsed in the last update.
		Time getLastDeltaTime( void );
		
		void setTime( Time argNewTime );

		/// Changes the rate at which game-time elapses compared to system-clock time.
		void setRate( Time argNewRate );

		/// Returns a number from 0.0 to 1.0 that indicates how far along the current
		/// time is from argStartTime to argEndTime.  Used for interpolation.
		double getFraction( Time argStartTime, Time argEndTime );

		/// Advances the time based on the elapsed system-clock time
		/// and the current rate.
		/// Returns the elapsed time since last update in game-seconds.
		Time update( void );

	private:
		void setupTimer( void );
		double getSystemTimerSeconds( void );
	private:

		Time myTime; //< Measured in milliseconds
		double myRate; //< Measured in game-millieconds per system-clock-milliseconds
		Time myLastTimestamp;  //< Measured in milliseconds
		Time myLastElapsedTime;

		double myClockRate; //< Measured in counts per second
		bool myHasPerformanceTimer; //< Determines how we can get the time (only used on win32)
	};

	// Simple timer class for profiling
	class CumulativeTimer
	{
	public:
		CumulativeTimer( void );
		void start( void );
		void stop( void );
		void reset( void );
		Time get( void );
	private:
		Time myTime;
		Tardis myTardis;
	};


	extern CumulativeTimer* gblTotalTimer;
	extern CumulativeTimer* gblPhysicsTimer;
	extern CumulativeTimer* gblControlTimer;
	extern CumulativeTimer* gblEmbryogenesisTimer;
	extern CumulativeTimer* gblIOTimer;


}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Tardis.hpp,v $
// Revision 1.3  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
