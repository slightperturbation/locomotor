/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
#ifndef ELSE_RENDERABLE_HPP
#define ELSE_RENDERABLE_HPP

// Copyright � 2004, Brian F. Allen, 
//
// Contact: vector@acm.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


/** \file 
\author Brian F. Allen (vector@acm.org)
*/

#include <Else.hpp>

namespace laetoli
{
	class Renderer;
}

namespace Else 
{

  /// Interface to define objects that can be queued for rendering.
  class Renderable
  {
  public:
    virtual void render( OpenGLRenderer* argRenderer ) = 0;
	virtual void render( laetoli::Renderer* argRenderer ) {}
    virtual ~Renderable() {}
  };
}

#endif

//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Renderable.hpp,v $
// Revision 1.4.2.1  2006/04/17 01:29:29  dexabraxis
// *** empty log message ***
//
// Revision 1.4  2004/06/28 00:08:06  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.3  2004/05/03 16:36:59  dexabraxis
// Integration with NEAT.  Basic UI in place. (possible threading bug)
//
// Revision 1.2  2004/04/30 23:29:36  dexabraxis
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////
