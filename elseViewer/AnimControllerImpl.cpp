/*
 *  AnimControllerImpl.cpp
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */

#include "AnimControllerImpl.hpp"

#include <Else.hpp>
#include <HumanoidArticulatedBody.hpp>
#include <PhysicsSimulator.hpp>
#include "OpenGLRenderer.hpp"
//#include <MySqlDB.hpp>
//#include <DbPerformance.hpp>
#include <iostream>

#include "UrlPerformance.h"

#include <vector>
#include <stdio.h>

using namespace Else;
using namespace std;

//#define DEFAULT_BODY_HEIGHT (1.75)
//#define DEFAULT_BODY_WEIGHT (75.0)
//#define DEFAULT_HIP_BREADTH (0.1625)  

AnimControllerImpl::AnimControllerImpl( void )
: _sim( new PhysicsSimulator ), 
  _body( new HumanoidArticulatedBody( _sim->getWorldID(), _sim->getSpaceID(),
                                     DEFAULT_BODY_HEIGHT, 
                                     DEFAULT_BODY_WEIGHT,
                                     DEFAULT_HIP_BREADTH ) ),
  _performanceId( 0 ),
  _isStatesValid( false ),
  _timeStep( 0.0165 ),
  _currentFrame( 0 )
{
    _sim->begin();
}

AnimControllerImpl::~AnimControllerImpl()
{
    delete _body;
    delete _sim;
}

void 
AnimControllerImpl::loadPerformance( long int argId )
{
    _isStatesValid = false;
    _performanceId = argId;
    getPerformanceStates( _states, argId );
    _timeStep = getPerformanceTimeStep();
    _isStatesValid = true;
}

void
AnimControllerImpl::setStateId( long int argId )
{
    if( argId >= 0 && argId < _states.size() )
    {
        _currentFrame = (size_t)argId;
    }
}

long int
AnimControllerImpl::getStateId( void )
{
    return _currentFrame;
}

void
AnimControllerImpl::draw( void )
{
    if( !_isStatesValid )
    {
        return;
    }

    glDisable( GL_CULL_FACE );
    //glDepthMask(GL_TRUE);
    
    // Turn On Fog
    glEnable(GL_FOG);
    float fogColor[3] = {0.05f,0.05f,0.05f}; 
    glFogfv(GL_FOG_COLOR, fogColor); // Set the fog color
    glFogf(GL_FOG_DENSITY, 0.05f);  // Thin the fog out a little
    
    
    // Draw floor
    glColor3f( 0.20, 0.20, 0.20 );
    glBegin( GL_QUADS );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-5.0f, 5.0f, -0.01f );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 5.0f, 5.0f, -0.01f );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 5.0f,-5.0f, -0.01f );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-5.0f,-5.0f, -0.01f );    
    glEnd();

    // Lighting
    float lightPos[4] = { 12.5f, 5.0f, 5.0f, 1.0f};
    float ambient[4] =  { 0.0f, 0.0f, 0.0f, 1.0f};
    float diffuse[4] =  { 1.0f, 1.0f, 1.0f, 1.0f };
    float specular[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
    
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos );
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular );

    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
    
    float black[4] = { 0, 0, 0, 0 };
    glMaterialfv( GL_FRONT, GL_AMBIENT, black );
    glMaterialfv( GL_FRONT, GL_SPECULAR, black );    
    
    glColor4d( 0, 0, 0, 0 );
    glPushMatrix();
    glScaled( 1.0, 1.0, 0.0001 );
    _renderer->render( _body );
    glPopMatrix();

    glColor4d( 0.25, 0.25, 0.65, 1.0 );
    _renderer->render( _body );
    
    glDisable( GL_LIGHTING );
    glEnable( GL_CULL_FACE );
}

void 
AnimControllerImpl::update( double dt )
{
    if( !_isStatesValid )
    {
        return;
    }
    if( _currentFrame >= _states.size() )
    {
        _currentFrame = 0;
    }    
    // TODO!!! compute the frame based on elapsed time
    update();    
    ++_currentFrame;
}

void
AnimControllerImpl::update( void )
{
    if( !_isStatesValid )
    {
        return;
    }
    if( _currentFrame >= _states.size() )
    {
        _currentFrame = 0;
    }    
    vector< BodyState >& currState = _states[_currentFrame];  // TODO -- map to current state using time, not index
    _body->setBodyState( currState );
}

std::vector< std::vector < Else::BodyState > >& 
AnimControllerImpl::getStates( void )
{
    return _states;
}

const char* 
AnimControllerImpl::getLastErrMsg( void )
{
    return _lastErr;
}

