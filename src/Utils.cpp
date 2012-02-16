/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// Copyright (c) 2004, Brian F. Allen,
//
// Contact: vector@acm.org
//

// Project Includes
#include "Utils.hpp"

// External Includes
#include <cmath>

namespace Else {

	double sigmoid( double aX )
	{
		return 1.0 / ( 1.0 + exp( -1.0 * aX ) );
	}

	double linear( double aX )
	{
		return aX;
	}
}

