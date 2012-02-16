#ifndef OPEN_GL_RENDERER_HPP
#define OPEN_GL_RENDERER_HPP

/*
 *  OpenGLRenderer.h
 *  elseViewer
 *
 *  Created by vector on 8/2/09.
 *  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
 *
 */
#include <Else.hpp>
#include <Renderable.hpp>
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/glu.h>

namespace Else
{
    class OpenGLRenderer
    {
    public:
        void render( RenderablePtr argRenderable );
        void render( Renderable* argRenderable );

        void drawBox( const gmtl::Vec3d argCenter,
                      const gmtl::Vec3d argDimensions, 
                      const double argOrientation[12] );
        void drawAxis( float argSize = 0.02 );
        void drawMarker( gmtl::Vec3d argPos );
    protected:
        
		/// Taken from GLUT source.
		void drawCube( float argSize );
        
		void setTransform( const gmtl::Vec3d argPos, const double R[12] );
    };
}


#endif
