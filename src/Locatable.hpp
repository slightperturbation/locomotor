/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_LOCATABLE_HPP
#define ELSE_LOCATABLE_HPP
//
// $Id: Locatable.hpp,v 1.3 2004/06/28 00:08:05 dexabraxis Exp $
// $Date: 2004/06/28 00:08:05 $
//
// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>

// External Includes

namespace Else 
{

	/// An interface for objects that can be located in world-space.
	class Locatable
	{
	public:
		/// The position of the object in World-space (aka, global coordinates).
		virtual gmtl::Vec3d getPosition( void ) = 0;
		virtual gmtl::Quatd getOrientation( void ) = 0;
		/// Virtual dtor to allow inheritance.  Pure to make class abstract.
		virtual ~Locatable() {}
	private:
	};

}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Locatable.hpp,v $
// Revision 1.3  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
