/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_SEGMENT_HPP
#define ELSE_SEGMENT_HPP
//
// $Id: Segment.hpp,v 1.1 2004/05/12 09:43:48 dexabraxis Exp $
// $Date: 2004/05/12 09:43:48 $
//
// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>


// External Includes
#include <ode/ode.h>

namespace Else 
{
  class Segment : public Renderable
  {

  private:
    dJointID myJoint;

  };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: Segment.hpp,v $
// Revision 1.1  2004/05/12 09:43:48  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
//
//////////////////////////////////////////////////////////////////////////

