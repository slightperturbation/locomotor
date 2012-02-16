/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_FITNESSFUNCFACTORY_HPP
#define ELSE_FITNESSFUNCFACTORY_HPP
//
// $Id: FitnessFuncFactory.hpp,v 1.3 2004/06/28 01:51:17 dexabraxis Exp $
// $Date: 2004/06/28 01:51:17 $
//
// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>
#include <string>

namespace Else 
{
    /// Factory that creates a FitnessFunction of the type specified.
    /// To support novelty search, all FitnessFunctions must belong to
    /// a PopulationFitness class.
    class FitnessFuncFactory
    {
    public:
        virtual ~FitnessFuncFactory() {}
        FitnessFuncPtr make( PopulationFitnessPtr argParent, const std::string& argFitnessType );
    };
}

#endif
