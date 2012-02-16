/// \author Brian "Dexter" Allen (vector@acm.org)
/// Copyright 2009. 

#include <RibRenderer.hpp>

#include <ri.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <fstream>


laetoli::RibRenderer
::RibRenderer( int argCameraPos )
: OfflineRenderer(),
  myCameraPos( argCameraPos )
{ 
}



laetoli::RibRenderer
::~RibRenderer( )
{
    // delete tmp fiel
    reset();
}

void 
laetoli::RibRenderer
::reset( void )
{

}



void
laetoli::RibRenderer
::pushMatrix( void )
{
	RiTransformBegin();
}

void
laetoli::RibRenderer
::popMatrix( void )
{
	RiTransformEnd();
}
void 
laetoli::RibRenderer
::beginAnimation( void )
{
    
    // Enable GZIP compression -- EXPERIMENTAL
    //int compression = 1; 
    //RiOption(RI_RIB,RI_COMPRESSION,"gzip",NULL]; 
    
    /*
    iqglo_ribFP = fopen( glo_currentJob.ribFileName.asChar(), "w" ); 
    if ( liqglo_ribFP ) { 
        RtInt ribFD = fileno( liqglo_ribFP ); 
        RiOption( ( RtToken )"rib", ( RtToken )"pipe", &ribFD, RI_NULL ); 
    } else { 
        cerr << ( "Error opening rib - writing to stdout.\n" ); 
    } 
    RiBegin( RI_NULL ); 
    */

	beginRIB();
    
	// Setup Search Path
	const char* shaderPath = ".:./data:%PIXIEHOME%/shaders:%SHADERS%";
	RiOption( "searchpath", "string shader", (RtPointer)&shaderPath, RI_NULL );


	// pixie doesn't support imager
	//RtColor bg = {1.0, 1.0, 1.0};
	//RiImager("background", "color", (RtPointer)bg, RI_NULL);

	RtColor color;
	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;
	//RiOption( "indirect", "color background", (RtPointer)&color, RI_NULL );


	int samples = 4;
	int xDim = 1280;  
	int yDim = 720;
    /*
	if( isPreview() )
	{
		samples = 2;
		xDim = 320;
		yDim = 240;
	}
*/
	RiFormat( xDim, yDim, 1.0f );
	RiPixelSamples( samples, samples );
}

void 
laetoli::RibRenderer
::endAnimation( void )
{
	RiEnd();
}

void
laetoli::RibRenderer
::beginFrame( int argFrameNumber )
{	
	std::stringstream ss ;
	//"_#h_#1n_#4f"  pixie is supposed to support name substitutions, but it doesn't seem to work
    
    // grab the filename after any directories
//	ss << myFilename.substr( myFilename.find_last_of("/\\") + 1 );
    ss << myName;
    ss << "_" << std::setfill('0') << std::setw(4) << argFrameNumber << ".png";
	RiFrameBegin( argFrameNumber );	
	RiDisplay( (RtString) (ss.str()).c_str(), "file", RI_RGBA, RI_NULL );
	//RiDisplay( "+framebuffer", RI_FRAMEBUFFER, RI_RGB, RI_NULL );

	if( myCameraPos == 0 )
	{
		// Side-view
		RiRotate( -90, 1, 0, 0 );
		RiTranslate( -0.5, 0, 0 ); 
		RiRotate( -5, 1, 0, 0 ); 
		RiTranslate( 0, 0, -0.85 );
		RiTranslate( 0, -2.3, 0 );
		RiRotate( -90, 0, 0, 1 );
	}
	else if( myCameraPos == 1 )
	{
		// "Three-quarters" perspective on staring position
		RiRotate( -90, 1, 0, 0 );
		RiTranslate( -1.5, 0, 0 ); 
		RiRotate( -5, 1, 0, 0 ); 
		RiTranslate( 0, 0, -0.85 );
		RiTranslate( 0, -4, 0 );
		RiRotate( -60, 0, 0, 1 );
	}
	else if( myCameraPos == 2 )
	{
		// Starting Position "beauty shot" -- 3/4th and below waist
		RiRotate( -90, 1, 0, 0 );
		RiRotate( 2, 1, 0, 0 ); 
		RiRotate( -30, 0, 0, 1 );
		RiTranslate( 1.2, -1.9, -0.65 );
	}
	else if( myCameraPos == 3 )
	{
		// Else 1.0, far-away viewpoint
		RiRotate( 15.0f, 1.0f, 0.0f, 0.0f );
		RiTranslate( 0.0f, 0.0f, 5.0f);
		RiRotate( -20.0f, 1.0f, 0.0f, 0.0f );
		RiTranslate( -2.0f, 0.0f, 0.0f );

		RiRotate( -120.0f, 1.0f, 0.0f, 0.0f );
		RiTranslate( 2.0f, -5.0f, 1.0f );
	}

	RtFloat fov = 45.0f;
	RiProjection( RI_PERSPECTIVE, "fov", &fov, RI_NULL );

	RiWorldBegin();

	// Setup background color using a sky-dome
	
	RiAttributeBegin();
	setColor( 1, 1, 1 );  // all white
	RiOrientation( RI_INSIDE );
	RiSurface( "constant", RI_NULL );
	RiSphere( 25, -25, 25, 360, RI_NULL );
	RiAttributeEnd();
	

	// Lights or GI
    /*
	if( isPreview() )
	{
		setupLights();
	}
	else
     */
	{
     
		setupGI( 0 );
	}

	// Floor
	{
		RtFloat samples = 4096.0f;
		RtToken samplesToken = RiDeclare( "samples", "uniform float" );
		//RtFloat maxDist = 2.0f;
		//RtToken distToken = RiDeclare( "maxdist", "uniform float" );
		RiSurface( "occludeSurf", samplesToken, (RtPointer)&samples, //distToken, (RtPointer)&maxDist, 
			RI_NULL );
	}


    /*if( isPreview() )
	{
		setColor( 0.95f, 0.95f, 0.95f );
	}
	else
     */
	{
		setColor( 0.875f, 0.875f, 0.875f );
	}

	drawBox( 20.0f, 20.0f, 0.01f );

	{
		RtFloat samples = 1024.0f;
		RtToken samplesToken = RiDeclare( "samples", "uniform float" );
		//RtFloat maxDist = 2.0f;
		//RtToken distToken = RiDeclare( "maxdist", "uniform float" );
		RiSurface( "occludeSurf", samplesToken, (RtPointer)&samples, //distToken, (RtPointer)&maxDist, 
			RI_NULL );
	}

	setColor( 0.65f, 0.65f, 0.825f );
}

void
laetoli::RibRenderer
::endFrame( void )
{
	RiWorldEnd();
	RiFrameEnd();
}

void
laetoli::RibRenderer
::beginBlend( void )
{

}

void 
laetoli::RibRenderer
::setColor( double argR, double argG, double argB, double argA )
{
	RtColor color;
	color[0] = argR;
	color[1] = argG;
	color[2] = argB;
	RiColor( color );
}

void
laetoli::RibRenderer
::multiplyMatrix( const double* argMatrix )
{
	// Argument is in column-major order, but Renderman wants row-major order;
	RtMatrix m;
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			m[i][j] = argMatrix[ (i*4)+j ];
		}
	}
	RiConcatTransform( m );
}

void
laetoli::RibRenderer
::init( void )
{
	// no global setup, only per-frame
}


void
laetoli::RibRenderer
::setupGI( int argPassCount )
{
	//Attribute "irradiance" "handle" "irradiance.cache" 
	//RtString irradianceCacheName = "irradiance.cache";
	//RiAttribute( "irradiance", (RtString)"handle", (RtPointer)&irradianceCacheName, RI_NULL );

	//Attribute "irradiance" "filemode" "w"
	// Write on first pass, read on subsequent passes
	//RtString irradianceCacheMode = argPassCount == 0 ? "w" : "r" ;
	//RiAttribute( "irradiance", (RtString)"filemode", (RtPointer)&irradianceCacheMode, RI_NULL );

	//////////////////////////////////////////////////////////////////////////
	// Setup GI 
	{
		int diffuse = 1;
		RiAttribute( RI_VISIBILITY, "diffuse", (RtPointer)&diffuse, RI_NULL );
	}
	//int diffuse = 1;
	//RiAttribute( "visibility", "int diffuse", (RtPointer)&diffuse, RI_NULL );

	//RtFloat traceBias = 0.0001f;
	//RiAttribute( "trace", "float bias", (RtPointer)&traceBias, RI_NULL );
	
	//RtFloat maxError = 0.0f;
	//RiAttribute( "irradiance", "float maxerror", (RtPointer)&maxError, RI_NULL );
	
	//RtFloat occlusionSamples = 128.0f;
	//RtToken numSamplesToken = RiDeclare( "numSamples", "uniform float" );
	
	//Attribute "cull" "hidden" 0
	int cull = 0;
	//RiAttribute( "cull", "hidden", (RtPointer)&cull, RI_NULL );

	//Attribute "cull" "backfacing" 0
	int backface = 0;
	//RiAttribute( "cull", "backfacing", (RtPointer)&backface, RI_NULL );

	//RtToken minRadiusToken = RiDeclare( "minR", "uniform float" );
	//RtFloat minSampleRadius = 0.1f;
	//RtToken maxRadiusToken = RiDeclare( "maxR", "uniform float" );
	//RtFloat maxSampleRadius = 0.15f;
	//RiLightSource( "ambientocclusionDex", 
	//	numSamplesToken, (RtPointer)&occlusionSamples,
	//	//minRadiusToken, (RtPointer)&minSampleRadius,
	//	//maxRadiusToken, (RtPointer)&maxSampleRadius,
	//	RI_NULL );
}

void
laetoli::RibRenderer
::setupLights( void )
{
	/*
	# LIGHTS
	Attribute "visibility" "transmission" "opaque" # |Os|shader|transparent
	LightSource "ambientlight" 1 "intensity" 0.1 
	LightSource "shadowdistant" 2 "from" [ 0.3 1.5 1 ] "to" [ 0 0 0 ] "shadowname" "raytrace" "intensity" [0.6] 
	LightSource "shadowdistant" 3 "from" [ -0.6 1.0 1 ] "to" [ 0 0 0 ] "shadowname" "raytrace" "intensity" [0.3] 
	LightSource "shadowdistant" 4 "from" [ -0.1 -1.0 0.5 ] "to" [ 0 0 0 ] "shadowname" "raytrace" "intensity" [0.1] 
	*/
	
	RtString opaqueVis[1] = { "opaque" };
	RiAttribute( "visibility", "transmission", (RtPointer)opaqueVis, RI_NULL);

	RtPoint from1 = { 0.3f, 1.5f, 1.0f };
	RtPoint from2 = {-0.6f, 1.0f, 1.0f };
	RtPoint from3 = {-0.1f,-1.0f, 0.5f };
	RtPoint to = { 0, 0, 0 };
	RtFloat intensity0 = 0.1;
	RtFloat intensity1 = 0.6;
	RtFloat intensity2 = 0.3;
	RtFloat intensity3 = 0.1;

	RtString shadowName[1] = { "shadowname" };
	RtString rayTrace[1] = { "raytrace" };
	RiLightSource( (RtString)"ambientlight", (RtString)"intensity", (RtPointer)&intensity0, RI_NULL );
	RiDeclare( "shadowname", "string" );
	RiDeclare( "raytrace", "string" );
	RiLightSource( (RtString)"shadowdistant", (RtString)"from", (RtPointer)from1, (RtString)"to", (RtPointer)to, (RtString)"intensity", (RtPointer)&intensity1, "shadowname", (RtPointer)rayTrace, RI_NULL );
	RiLightSource( (RtString)"shadowdistant", (RtString)"from", (RtPointer)from2, (RtString)"to", (RtPointer)to, (RtString)"intensity", (RtPointer)&intensity2, "shadowname", (RtPointer)rayTrace, RI_NULL );
	RiLightSource( (RtString)"shadowdistant", (RtString)"from", (RtPointer)from3, (RtString)"to", (RtPointer)to, (RtString)"intensity", (RtPointer)&intensity3, "shadowname", (RtPointer)rayTrace, RI_NULL );
}

void 
laetoli::RibRenderer
::drawBox( double argX, double argY, double argZ )
{
	RiTransformBegin();
	RiAttributeBegin();
	RiScale( argX, argY, argZ );

	// Materials
	//RtColor color = { 0.9f, 0.9f, 0.9f };
	//RiColor( color );

	// Geometry

	// Top
	RtPoint cubeTopPoints[4] 
            = {	{0.5,	0.5f,	-0.5f},
                {0.5,	0.5f,	 0.5f},
                {-0.5,	0.5f,	 0.5f},
                {-0.5,	0.5f,	-0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeTopPoints, RI_NULL );

	RtPoint cubeBottomPoints[4] 
            = {	{-0.5,	-0.5f,	-0.5f},
                {0.5,	-0.5f,	-0.5f},
                {0.5,	-0.5f,	 0.5f},
                {-0.5,	-0.5f,	 0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeBottomPoints, RI_NULL );

	RtPoint cubeBackPoints[4] 
            = {	{-0.5,	0.5f,	 0.5f},
                {0.5,	0.5f,	 0.5f},
                {0.5,	-0.5f,	 0.5f},
                {-0.5,	-0.5f,	 0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeBackPoints, RI_NULL );

	RtPoint cubeRightPoints[4] 
            = { {	0.5,	0.5f,	 0.5f},
                {0.5,	0.5f,	 -0.5f},
                {0.5,	-0.5f,	 -0.5f},
                {0.5,	-0.5f,	 0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeRightPoints, RI_NULL );

	RtPoint cubeLeftPoints[4] 
            = {	{-0.5,	0.5f,	 -0.5f},
                 {-0.5,	0.5f,	  0.5f},
                  {-0.5,	-0.5f,	  0.5f},
                   {-0.5,	-0.5f,	 -0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeLeftPoints, RI_NULL );
	RtPoint cubeFrontPoints[4] 
            = {	 {0.5,	0.5f,	 -0.5f},
                 {-0.5,	0.5f,	 -0.5f},
                 {-0.5,	-0.5f,	 -0.5f},
                 {0.5,	-0.5f,	 -0.5f} };
	RiPolygon( 4, RI_P, (RtPointer)cubeFrontPoints, RI_NULL );

	RiAttributeEnd();
	RiTransformEnd();
}


void 
laetoli::RibRenderer
::drawVector( Vec3d argOrigin, Vec3d argVector )
{
	//Color [1.0 0.0 0.0]
	//Translate 0 1 0  # from X Y Z
	//#Rotate 90 0 1 0
	//Cylinder 0.04 0 1 360
	//Translate 0 0 1
	//Cone 0.1 0.1 360

	if( argOrigin[1] != argVector[1] )
	{
		return;
	}

	RiTransformBegin();
	RiAttributeBegin();

	Vec3d axis( 0, 1, 0 );
	//cross( axis, Vec3d( 0, 0, 1 ), argVector );
	double angle = angleTo( Vec3d( 0, 0, 1 ), argVector );

	RiRotate( angle * 180.0 / M_PI, axis[0], axis[1], axis[2] );

	RiTranslate( argOrigin[0], argOrigin[1], argOrigin[2] );
	
	//double mag = argVector.length();
	//RiScale( mag, 1, 1 );

	// Materials
	//RiSurface( "plastic", RI_NULL );
	//RtColor color = { 0.9f, 0.9f, 0.9f };
	//RiColor( color );

	RiCylinder( 0.04, 0, 1, 360, RI_NULL );
	RiTranslate( 0, 0, 1 );
	RiCone( 0.1, 0.1, 360, RI_NULL );

	RiAttributeEnd();
	RiTransformEnd();
}

void laetoli::RibRenderer::write( std::ostream& out )
{
    assert( false );
    // TODO
}


/////////////////////////////////////////////////////////////////
laetoli::TmpRibRenderer
::TmpRibRenderer( int argCameraPos )
: RibRenderer( argCameraPos )
{
    char* tmpBuf = std::tmpnam( 0 );
    myFilename = std::string( tmpBuf );
}

void 
laetoli::TmpRibRenderer
::reset( void )
{
    // delete stored anim stream
    if( remove( myFilename.c_str() ) != 0 )
    {
        std::cerr << "Error deleting temporary RIB file: " << myFilename << "\n";
    }
}

void laetoli::TmpRibRenderer
::write( std::ostream& out )
{
    // RiEnd() must have been called.
    
    // read in temporary file and stream to out
    std::ifstream inFile( myFilename.c_str() );
    if( !inFile )
    {
        std::cerr << "unable to read temporary RIB file\n";
        return;
    }
    char ch;
    while( inFile && inFile.get(ch) )
    {
        out.put( ch );
    }
    inFile.close();
    reset();
}

void laetoli::TmpRibRenderer
::beginRIB( void )
{
    RiBegin( (RtString) myFilename.c_str() );
}

laetoli::FileRibRenderer
::FileRibRenderer( const std::string& argFilename, int argCameraPos )
: RibRenderer( argCameraPos ),
  myFilename( argFilename )
{ }

void
laetoli::FileRibRenderer
::beginRIB( void )
{
    if( myFilename.empty() )
    {
        RiBegin( NULL ); // push to stdout 
    }
    else
    {
        RiBegin( myFilename.c_str() );
    }
}

