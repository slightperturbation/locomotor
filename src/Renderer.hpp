/// \author Brian "Dexter" Allen (vector@acm.org)
/// Copyright 2009. 
#ifndef LAETOLI_RENDERER_HPP
#define LAETOLI_RENDERER_HPP

// Project Includes
#include <Else.hpp>
#include <Renderable.hpp>


#include <Math.hpp>
#include <iostream>
#include <list>

namespace laetoli
{
	/// Interface providing high-level commands for drawing the 3D scene.
	/// Designed with the intent of supporting Renderman Interface 
	/// Byte-stream (RIB) file output and OpenGL interactive display output.
	class Renderer
	{
	public:
        virtual ~Renderer() {}

		/// Setup renderer.  Should be called once, prior to any rendering.
		virtual void init( void ) = 0;

		virtual void render( Else::RenderablePtr argRenderable ) = 0;
		virtual void render( Else::Renderable* argRenderable ) = 0;
		virtual void renderScene( void ) = 0;
		virtual void addSceneRenderable( Else::RenderablePtr argRenderable ) = 0;

		/// Store a copy of the current transformation matrix.
		virtual void pushMatrix( void ) = 0;
		/// Discard the current transformation matrix and use the previous.
		virtual void popMatrix( void ) = 0;

		/// Draw a box at the origin with sides of length specified
		virtual void drawBox( double argX, double argY, double argZ ) = 0;
		void drawBox( double* argDim )
		{ drawBox( argDim[0], argDim[1], argDim[2] ); }

		/// Draw the given vector situated at the origin
		virtual void drawVector( Vec3d argOrigin, Vec3d argVector ) = 0;

		/// Multiply the given 4x4, column-major homogeneous transform
		/// against the current matrix transform.  Successive calls
		/// will be applied in-order.
		virtual void multiplyMatrix( const double[16] ) = 0;

		/// Call to designate the beginning of rendering of the current frame.
		virtual void beginFrame( int argFrameNumber = 0 ) = 0;

		/// Call to designate the end of rendering for the current frame.
		virtual void endFrame( void ) = 0;

		/// Begin drawing blended
		virtual void beginBlend( void ) = 0;

		/// Change the current drawing color
		virtual void setColor( double argR, double argG, double argB, double argA ) = 0;
	};

	/// Interface class for a renderer that saves to files rather than sending to the display.
	class OfflineRenderer : public Renderer
	{
	public:
		OfflineRenderer( void ) 
			: Renderer(),
              myName( "Render" )
		{ }
		virtual ~OfflineRenderer( ) { }

        virtual void renderScene( void )
        {
            for( std::list< Else::RenderablePtr >::iterator i = mySceneRenderables.begin();
                 i != mySceneRenderables.end(); ++i )
            {
                Else::RenderablePtr rp = *i;
                render( rp );
            }
        }

        virtual void addSceneRenderable( Else::RenderablePtr argRenderable )
        {
            mySceneRenderables.push_back( argRenderable );
        }

        virtual void beginAnimation( void ) = 0;
		virtual void endAnimation( void ) = 0;

		virtual void endFrame( void ) = 0;
		virtual void beginFrame( int argFrameNumber = 0 ) = 0;

        virtual void reset( void ) = 0;
        virtual void write( std::ostream& out ) = 0;
        
        virtual void setName( const std::string& name ) 
        {
            myName = name;
        }
        std::string myName;
	protected:
        std::list< Else::RenderablePtr > mySceneRenderables;
	};
}

#endif

