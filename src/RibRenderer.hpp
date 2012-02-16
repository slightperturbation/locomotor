/// $Id: RibRenderer.hpp,v 1.1.2.2 2006/04/20 09:39:51 dexabraxis Exp $
/// $URL: http://www.luminousbeings.com/svn/laetoli/trunk/Laetoli/graphics/RibRenderer.hpp $
/// \author Brian "Dexter" Allen (vector@acm.org)
/// Copyright 2009. 
#ifndef LAETOLI_RIBRENDERER_HPP
#define LAETOLI_RIBRENDERER_HPP

#include <Renderer.hpp>
#include <Renderable.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace laetoli
{
	class RibRenderer : public OfflineRenderer
	{
	public:
		RibRenderer( int argCameraPos = 0 );
		virtual ~RibRenderer();
		
		virtual void render( Else::RenderablePtr argRenderable )
		{
			argRenderable->render( this );
		}
		virtual void render( Else::Renderable* argRenderable )
		{
			argRenderable->render( this );
		}

		/// Setup base state that will be used throughout lifespan.  Should 
		/// be idempotent.
		virtual void init( void );

		/// Store a copy of the current transformation matrix.
		virtual void pushMatrix( void );
		/// Discard the current transformation matrix and use the previous.
		virtual void popMatrix( void );

		/// Draw a box sitting on the X-Z plane at the origin,
		/// with the cube is only in the positive Y-axis
		/// with the dimensions specified.
		virtual void drawBox( double argX, double argY, double argZ );

		/// Draw the given vector situated at the origin
		virtual void drawVector( Vec3d argOrigin, Vec3d argVector );

		/// Multiply the given 4x4, column-major homogeneous transform
		/// against the current matrix transform.  Successive calls
		/// will be applied in-order.
		virtual void multiplyMatrix( const double* );

		/// Call to designate the end of rendering for the current frame.
		virtual void endFrame( void );
		/// Call to designate the beginning of rendering of the current frame.
		virtual void beginFrame( int argFrameNumber = 0 );

		virtual void beginAnimation( void );
		virtual void endAnimation( void );
        
        virtual void reset( void );
        
		/// Begin drawing blended
		virtual void beginBlend( void );

		virtual void setColor( double argR, double argG, double argB, double argA = 0 );

        void write( std::ostream& out );
    protected:
        virtual void beginRIB( void ) = 0;
	private:
		void setupGI( int argPassCount );
		void setupLights( void );
        int myCameraPos;
	};
    
    class TmpRibRenderer : public RibRenderer
    {
    public:
        TmpRibRenderer( int argCameraPos = 0 );
        
        virtual ~TmpRibRenderer() { }
        
        virtual void reset( void );        
        virtual void write( std::ostream& out );
    protected:
        virtual void beginRIB( void );
    private:
        std::string myFilename;        
    };
    
    class FileRibRenderer : public RibRenderer
    {
    public:
        FileRibRenderer( const std::string& argFile = "", int argCameraPos = 0 );
        ~FileRibRenderer() { }
        virtual void reset( void ) { }     
        virtual void write( std::ostream& out ) { }
    protected:
        virtual void beginRIB( void );
    private:
        std::string myFilename;
    };
}

#endif
