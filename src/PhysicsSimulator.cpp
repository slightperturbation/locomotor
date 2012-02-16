/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// Copyright (c) 2004, Brian F. Allen,
//
// Contact: vector@acm.org
//

// Project Includes
#include <Else.hpp>
#include <PhysicsSimulator.hpp>
#include <ArticulatedBody.hpp>

// External Includes


bool Else::PhysicsSimulator::ourIsInitialized = false;

void nearCallback( void *data, dGeomID o1, dGeomID o2 )
{
  int i,n;
  dBodyID b1, b2;

  b1 = dGeomGetBody( o1 );
  b2 = dGeomGetBody( o2 );
    Else::PhysicsSimulator* sim = static_cast< Else::PhysicsSimulator* >(data);
    dJointGroupID collisionID = sim->getContactJointGroupID();    
    /*
    dContact contactTmp;
    
    if (b1 && b2 && dAreConnected (b1,b2)) return;
    
    contactTmp.surface.mode = 0;
    contactTmp.surface.mu = 0.1;
    contactTmp.surface.mu2 = 0;
    if (dCollide (o1,o2,1,&contactTmp.geom,sizeof(dContactGeom))) {
        dJointID c = dJointCreateContact (worldID,collisionID,&contactTmp);
        dJointAttach (c,b1,b2);
    }
    return;
*/
  // exit without doing anything if the two bodies are connected by a joint
  if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) 
  {
    return;
  }

	if( b1 && b2 )
	{
		Else::ArticulatedBody* artBod1 = static_cast<Else::ArticulatedBody* >(dBodyGetData( b1 ));
		Else::ArticulatedBody* artBod2 = static_cast<Else::ArticulatedBody* >(dBodyGetData( b2 ));
		if( sim && !sim->getAllowInternalCollisions() )
		{
			if( artBod1 == artBod2 && artBod1 != 0 && artBod2 != 0) 
			{
				// both are part of an articulatedBody, no collisions
//				return;
			}
		}
	}
	
	if( b1 )
	{
		Else::ArticulatedBody* artBod1 = static_cast<Else::ArticulatedBody* >(dBodyGetData( b1 ));
		if( artBod1 && sim->isGroundGeom( o2 ) )
		{
			Else::BodySegmentEnum type1 = static_cast<Else::BodySegmentEnum>((long int)dGeomGetData( o1 ));
			artBod1->recordCollision( type1, sim->getElapsedTime() );		
		}
	}
	if( b2 )
	{
		Else::ArticulatedBody* artBod2 = static_cast<Else::ArticulatedBody* >(dBodyGetData( b2 ));
		if( artBod2 && sim->isGroundGeom( o1 ) )
		{
			Else::BodySegmentEnum type2 = static_cast<Else::BodySegmentEnum>((long int)dGeomGetData( o2 ));
			artBod2->recordCollision( type2, sim->getElapsedTime() );
		}
	}


  if( sim->getWorldID() == (dWorldID)(-1) || collisionID == (dJointGroupID)(-1) )
  {
    std::cerr << "Ignoring collision between objects outside "
      << "of known world! (probably an error!)\n";
    return;
  }

  const int N = 100;
  dContact contact[N];
  n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
  if (n > 0) {
    //	einvigi::Log::getInstance().debug() << "Collision at " << n << " points"
    //		<< " between " << b1 << " and " << b2 << "\n";
    for (i=0; i<n; i++) {

/*
 * From BoxTest.cpp ODE examples	
			contact[i].surface.mode = dContactBounce | dContactSoftCFM;
			contact[i].surface.mu = dInfinity;
			contact[i].surface.mu2 = 0;
			contact[i].surface.bounce = 0.1;
			contact[i].surface.bounce_vel = 0.1;
			contact[i].surface.soft_cfm = 0.01;
*/
/*
      contact[i].surface.mode = dContactApprox1; 
      contact[i].surface.mu = dInfinity;
*/
/*
      contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactBounce;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.soft_erp = 0.4;
      contact[i].surface.soft_cfm = 0.00001;
      contact[i].surface.bounce = 0.1;

*/
			/*
      contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
      dContactSoftERP | dContactSoftCFM | dContactApprox1;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.slip1 = 0.01;
      contact[i].surface.slip2 = 0.01;
      contact[i].surface.soft_erp = 0.4;
      contact[i].surface.soft_cfm = 1e-9;
*/
/*
      contact[i].surface.mode = dContactBounce | dContactSoftCFM;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.mu2 = 0;
      contact[i].surface.bounce = 0.7;
      contact[i].surface.bounce_vel = 0.4;
      contact[i].surface.soft_cfm = 0.01;
*/

        //GOOD
/*		contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1;
		contact[i].surface.mu = dInfinity; //0.9;
		contact[i].surfact.mu2 = 0;
		contact[i].surface.soft_erp = sim->getCollisionERP();//0.4;
		contact[i].surface.soft_cfm = sim->getCollisionCFM();//1e-9;
*/
		contact[i].surface.mode = dContactApprox1;
		contact[i].surface.mu = dInfinity; //0.9;
		contact[i].surface.mu2 = 0;
		contact[i].surface.soft_erp = sim->getCollisionERP();//0.4;
		contact[i].surface.soft_cfm = sim->getCollisionCFM();//1e-9;

		dJointID c = dJointCreateContact( sim->getWorldID(), collisionID, &contact[i] );
      //b1 = dGeomGetBody(contact[i].geom.g1);
      //b2 = dGeomGetBody(contact[i].geom.g2);


      // ORDER MATTERS for attachment -- should it??
      // according to 0.39 docs, it seems like it should behave
      // symmetrically, but it clearly doesn't!
      dJointAttach (c, b1, b2);
    }
  }
}
