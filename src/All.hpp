/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_ALL_HPP
#define ELSE_ALL_HPP
//
// $Id: All.hpp,v 1.1 2004/04/30 23:29:36 dexabraxis Exp $
// $Date: 2004/04/30 23:29:36 $
//
// Copyright (c) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//



#include <Else.hpp>
#include <ArticulatedBody.hpp>
#include <Controller.hpp>
#include <ElseException.hpp>
#include <Evaluation.hpp>
#include <Evolver.hpp>
#include <Locatable.hpp>
#include <NeatController.hpp>
#include <Renderable.hpp>
#include <Singleton.hpp>
#include <Tardis.hpp>
#include <NeatEvolver.hpp>
#include <BalanceFitnessFunc.hpp>
#include <FitnessFuncFactory.hpp>
#include <FitnessFunc.hpp>
#include <PhysicsSimulator.hpp>

#ifdef COCOA_GUI
#include <OpenGLRenderer.hpp>
#endif

#ifdef ELSE_GUI
#include <GLRenderer.hpp>
#include <EvolverUI.hpp>
#include <SimulationWindow.hpp>
#endif

#endif
