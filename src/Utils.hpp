/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef ELSE_UTILS_HPP
#define ELSE_UTILS_HPP
//
// $Id: Utils.hpp,v 1.3 2004/07/26 21:57:40 dexabraxis Exp $
// $Date: 2004/07/26 21:57:40 $
//
// Copyright (c) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes

// External Includes

#include <cmath>
#include <cstdlib>

namespace Else {

	// Activation Functions

	double sigmoid( double aX );
	double linear( double aX );

	// Random distributions
	
	template< typename T >
	void swap( T& a, T& b )
	{
		T t = a;
		a = b;
		b = t;
	}

	template< typename NumT >
	NumT randomUniform( NumT argMin, NumT argMax )
	{
		if( argMax < argMin ) {
			swap( argMax, argMin );
		}
		double ratio = rand() / (RAND_MAX+1.0);
		double range = argMax - argMin;
		return (NumT)( argMin + (NumT)(range * ratio) );
	}

	// TODO : implement gaussian for real (currently hacked)
	template< typename NumT >
	NumT randomGaussian( NumT aMu, NumT aSigma )
	{
		int n = 10;
		NumT x = 0;
		for( int i = 0; i < n; ++i ) {
			x += randomUniform( (NumT)0, aSigma );
		}
		return x + aMu;
	}
}

#endif
