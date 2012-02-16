/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_SINGLETON_HPP
#define ELSE_SINGLETON_HPP

// Based on code by Scott Bilas, Gas Powered Games

/** \file 
\author Scott Bilas (scottb at drizzledotcom)
*/

// External Includes
#include <cassert>

namespace Else
{
	template <typename T>
	class Singleton 
	{
	public:
		Singleton( void )
		{
			assert( !mySingleton );
			// difference in location between sizes of the two types
			// needed for cases of multiple inheritence.
			int offset 
				= (int)(T*)1 - (int)(Singleton<T>*)(T*)1; 
			mySingleton = (T*)((int)this + offset);
		}

		~Singleton( void )
		{
			assert( mySingleton );
			mySingleton = 0;
		}

		static T& getInstance( void )
		{
			assert( mySingleton );
			return *mySingleton;
		}

		static T* getInstancePtr( void )
		{
			return mySingleton;
		}
	private:
		static T* mySingleton;
	};

	template <typename T> T* Singleton <T>::mySingleton = 0;

}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Singleton.hpp,v $
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////

