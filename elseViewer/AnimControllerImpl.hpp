#ifndef ANIM_CONTROLLER_IMPL_HPP
#define ANIM_CONTROLLER_IMPL_HPP
/*
 *  AnimControllerImpl.hpp
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <vector>
#include <BodyState.hpp>
#include <DbPerformance.hpp>

namespace Else
{
    class PhysicsSimulator;
    class ArticulatedBody;
    class OpenGLRenderer;
    
    /// C++ Implementation of Objective-C++ AnimController Class
    /// All Else & C++ stuff goes here.
    class AnimControllerImpl
    {
    public:
        AnimControllerImpl( void );
        ~AnimControllerImpl();
        
        void loadPerformance( long int argId );
        void setStateId( long int argId );
        long int getStateId( void );
        void draw( void );
        void update( void );
        void update( double dt );
        
        std::vector< std::vector < Else::BodyState > >& getStates( void );
        const char* getLastErrMsg( void );
        
    private:
        char _lastErr[1024];
        PhysicsSimulator* _sim;
        ArticulatedBody* _body;
        OpenGLRenderer* _renderer;
        long int _performanceId;
        std::vector< std::vector< Else::BodyState > > _states;
        bool _isStatesValid;
        double _timeStep;
        size_t _currentFrame;
    };

}

#endif
