/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ELSE_PHYSICSSIMULATOR_HPP
#define ELSE_PHYSICSSIMULATOR_HPP
//
// $Id: PhysicsSimulator.hpp,v 1.14.2.2 2006/04/20 09:39:51 dexabraxis Exp $
// $Date: 2006/04/20 09:39:51 $
//
// Copyright (C) 2004, Brian F. Allen
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>
#include <HumanoidArticulatedBody.hpp>
#include <CollisionBox.hpp>
#include <GLTools.h>
#include <RibRenderer.hpp>

// External Includes
#include <ode/ode.h>
#include <boost/foreach.hpp>

void nearCallback( void *data, dGeomID o1, dGeomID o2 );

namespace Else 
{
    
    
	/// Encapsulates the physical simulation.
	class PhysicsSimulator
        {
        public:
            PhysicsSimulator( void )
            : myStepSize( DEFAULT_TIME_STEP ),
              myUseFastStep( DEFAULT_USE_FAST_STEP ),
              myAllowInternalCollisions( DEFAULT_ALLOW_INTERNAL_COLLISIONS ),
              myFastSubSteps( DEFAULT_TIME_SUBSTEPS ),
              myCollisionERP( DEFAULT_COLLISION_ERP ),
              myCollisionCFM( DEFAULT_COLLISION_CFM ),
              myFrame( 1 )
            {
                init();
                setup();
            }
            
            ~PhysicsSimulator( void )
            {
                clearRenderers();
                dJointGroupDestroy( myContactJointGroupID );
                
                // then remove world
                dSpaceDestroy( mySpaceID );
                dWorldDestroy( myWorldID );
            }
            /// Init is idempotent-- call with each constructor to ensure
            /// initialization of ODE
            static void init( void )
            {
                if( !ourIsInitialized )
                {
                    ourIsInitialized = true;
                    dInitODE2(0);
                    dAllocateODEDataForThread(dAllocateMaskAll);
                }
            }

            void clearRenderers( void )
            {
                BOOST_FOREACH( laetoli::OfflineRenderer* r, myRenderers )
                {
                    delete r;
                }
                myRenderers.clear();
            }

            void addRenderer( laetoli::OfflineRenderer* r )
            {
                if( r )
                {
                    myRenderers.push_back( r );
                }
            }

            std::vector< laetoli::OfflineRenderer* > getRenderers( void )
            {
                return myRenderers;
            }

            dWorldID getWorldID( void ) { return myWorldID; }
            dSpaceID getSpaceID( void ) { return mySpaceID; }

            bool isGroundGeom( dGeomID argID )
            {
                return myGroundGeomID == argID;
            }
            double getGravity( void )
            {
                dVector3 tmp;
                dWorldGetGravity( myWorldID, tmp );
                return tmp[2];
            }

            void setGravity( double argZ )
            {
                dWorldSetGravity( myWorldID, 0, 0, argZ );
            }
            void setCFM( double argCFM )
            {
                dWorldSetCFM( myWorldID, argCFM );
            }
            double getCFM( void )
            {
                return dWorldGetCFM( myWorldID );
            }
            void setERP( double argERP )
            {
                dWorldSetERP( myWorldID, argERP );
            }
            double getERP( void )
            {
                return dWorldGetERP( myWorldID );
            }

            void setCollisionCFM( double argCFM )
            {
                myCollisionCFM = argCFM;
            }
            double getCollisionCFM( void )
            {
                return myCollisionCFM;
            }
            void setCollisionERP( double argERP )
            {
                myCollisionERP = argERP;
            }
            double getCollisionERP( void )
            {
                return myCollisionERP;
            }


            void setUseFastStep( bool arg )
            {
                myUseFastStep = arg;
            }
            bool getUseFastStep( void )
            {
                return myUseFastStep;
            }
            void setTimeStep( double argTimeStep )
            {
                myStepSize = argTimeStep;
            }
            double getTimeStep( void )
            {
                return myStepSize;
            }
            void setFastSubSteps( int arg )
            {
                myFastSubSteps = arg;
            }
            int getFastSubSteps( void )
            {
                return myFastSubSteps;
            }
            bool getAllowInternalCollisions( void )
            {
                return myAllowInternalCollisions;
            }
            void setAllowInternalCollisions( bool arg )
            {
                myAllowInternalCollisions = arg;
            }

            void begin( int generation = 0, int trial = 0 )
            {
                std::stringstream ss;
                ss << "Gen-" << generation << "_Trial-" << trial;
                BOOST_FOREACH( laetoli::OfflineRenderer* r, myRenderers )
                {
                    r->setName( ss.str() );
                    r->beginAnimation();
                }

                myCurrTime = 0;

                /// OPTIMIZATION - 2 Sept 09 - amazingly, building the body is very slow, almost as much as the sim in early stages!
                /// replacing the reset with setting to the initial state, by wary of this causing problems!

                /// OLD:
                //myBody->reset();

                /// NEW:  in Evalution::operator(), there's a call to myBody->setState( ... )
            }

            void end( void )
            {
                BOOST_FOREACH( laetoli::OfflineRenderer* r, myRenderers )
                {
                    r->endAnimation();
                }

                myCurrTime = 0;
                //myBody->reset();
                /*
                 This way is just too slow...
                 if( !myBody.unique() )
                 {
                 throw "Possible lingering reference to old body!";
                 }
                 myBody.reset();
                 myBody = ArticulatedBodyPtr( new ArticulatedBody( myWorldID, mySpaceID ) );
                 */
            }

            dJointGroupID getContactJointGroupID( void )
            {
                return myContactJointGroupID;
            }

            void getStatics( Renderables& outRenderables )
            {
                if( myGround )
                {
                    outRenderables.push_back( myGround );
                }
            }

            void step( void )
            {
                myCurrTime += myStepSize;
                try {
                    dSpaceCollide( mySpaceID, this, &nearCallback );
//                    if( myUseFastStep )
//                    {
//                        //dWorldStepFast( myWorldID, myStepSize, myFastSubSteps  );
//                    }
//                    else {
                    dWorldStep( myWorldID, myStepSize );
//                      }
                    // remove all contact joints
                    dJointGroupEmpty( myContactJointGroupID );
                }
                catch( std::exception e )
                {
                    std::cerr << "Caught an exception during world update:  \""
                    << e.what() << "\"\n\tSlowing down!\n";
                    myStepSize *= 0.85f;
                }
                catch( ... )
                {
                    std::cerr << "Caught an exception during world update. Ignoring!\n";
                }
                BOOST_FOREACH( laetoli::OfflineRenderer* renderer, myRenderers )
                {
                    if( renderer )
                    {
                        renderer->beginFrame( myFrame++ );
                        renderer->renderScene();
                        renderer->endFrame();
                    }
                }
            }

            //@ DEPRECATED
            void saveAnim( const std::string& argPrefix, int argW, int argH, int argCameraPos, bool argPreview )
            {
                assert( false );
                /*
                myAnimPrefix = argPrefix;
                myIsSavingAnim = true;
                myImageH = argH;
                myImageW = argW;

                laetoli::OfflineRenderer* renderer = new laetoli::RibRenderer( argCameraPos, argPreview);
                renderer->setBaseFilename( argPrefix + "RIB" );
                renderer->beginAnimation();
                myOfflineRenderers.push_back( renderer );
                */
            }
            /*
            void startAnim( void )
            {
                BOOST_FOREACH( laetoli::OfflineRenderer* renderer, myRenderers )
                {
                    renderer->beginAnimation();
                }
            }

            void stopAnim( void )
            {

                BOOST_FOREACH( laetoli::OfflineRenderer* renderer, myRenderers )
                {
                    if( renderer )
                    {
                        renderer->endAnimation();
                    }
                }
            }
             */

            /*  GUI will still be using this, may need to re-enable at some point
            void saveImage( const std::string& argPrefix )
            {
                //saveScreenImage( argPrefix, myImageW, myImageH );
                if( myOfflineRenderer )
                {
                    myOfflineRenderer->beginFrame( myFrame++ );
                    myOfflineRenderer->render( getBody() );
                    myOfflineRenderer->endFrame();
                }
            }
            */

            /*
            void recordJointTraces( const std::string& argFileName )
            {
                if( myBody )
                {
                    myBody->recordJointTraces( argFileName );
                }
            }

            void endJointTraces( void )
            {
                if( myBody )
                {
                    myBody->endJointTraces();
                }
            }

            void recordNeuralTraces( const std::string& argFileName )
            {
                if( myBody )
                {
                    myBody->recordNeuralTraces( argFileName );
                }
            }
            void endNeuralTraces( void )
            {
                if( myBody )
                {
                    myBody->endNeuralTraces();
                }
            }
            */
//            void addHarnessForce( double argProgress )
//            {
//                myBody->addHarnessForce( argProgress, getTimeStep() );
//            }

//            void applyPerturbation( double argStrength, double argDirection )
//            {

//            }

            double getElapsedTime( void ) { return myCurrTime; }


        private:
            PhysicsSimulator( const PhysicsSimulator& other )
            {
                throw "deprecated!";
                
                // deep copy myBody
                //setup();
                //myStepSize = other.myStepSize;
                //myUseFastStep = other.myUseFastStep;
                //myAllowInternalCollisions = other.myAllowInternalCollisions;
                //myCollisionERP = other.myCollisionERP;
                //myCollisionCFM = other.myCollisionCFM;
                //myFrame = other.myFrame;
                //myCurrTime = other.myCurrTime;
                //myBody = new HumanoidArticulatedBody( myWorldID, mySpaceID, *other.myBody );
            }

            /// Creates and configures the underlying physics engine
            /// init() must have already been called
            void setup( void )
            {
                myWorldID = dWorldCreate();
                mySpaceID = dSimpleSpaceCreate( 0 );
                myContactJointGroupID = dJointGroupCreate( 0 );
                
                dWorldSetGravity( myWorldID, 0, 0, DEFAULT_GRAVITY );
                dWorldSetERP( myWorldID, DEFAULT_ERP );
                dWorldSetCFM( myWorldID, DEFAULT_CFM );
                //dCreatePlane( mySpaceID, 0, 0, 1, 0 );
                
                CollisionBox* cb = new CollisionBox( mySpaceID, 32, 32, 2.0 );
                myGroundGeomID = cb->getGeomID();
                cb->setPosition( gmtl::Vec3d( 0, 0, -1.005 ) );
                myGround = RenderablePtr( cb );
                myCurrTime = 0;
            }
            
        private:
            static bool ourIsInitialized;
            dWorldID myWorldID;
            dSpaceID mySpaceID;
            dJointGroupID myContactJointGroupID;
            double myStepSize;
            RenderablePtr myGround;
            dGeomID myGroundGeomID;
            double myCurrTime;
            bool myUseFastStep;
            bool myAllowInternalCollisions;
            int myFastSubSteps;
            //std::string myAnimPrefix;
            //bool myIsSavingAnim;
            //int myImageW, myImageH;
            double myCollisionERP;
            double myCollisionCFM;
            std::vector< laetoli::OfflineRenderer* > myRenderers;
            int myFrame;
        };
    
}

#endif
