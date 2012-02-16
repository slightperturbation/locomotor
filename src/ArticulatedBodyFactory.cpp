#include "ArticulatedBodyFactory.hpp"
#include "AcrobotArticulatedBody.hpp"
#include "PhysicsSimulator.hpp"

Else::HumanoidArticulatedBodyFactory
::HumanoidArticulatedBodyFactory( double argHeight,
                                  double argWeight,
                                  double argHipBreadth,
                                  double argAnkleStrength,
                                  double argFootLengthPerHeight,
                                  double argFootWidthPerHeight )
: myHeight( argHeight ),
  myWeight( argWeight ),
  myHipBreadth( argHipBreadth ),
  myAnkleStrength( argAnkleStrength ),
  myFootLengthPerHeight( argFootLengthPerHeight ),
  myFootWidthPerHeight( argFootWidthPerHeight )
{
    // noop
}

Else::ArticulatedBodyPtr
Else::HumanoidArticulatedBodyFactory
::create( PhysicsSimulatorPtr argSim )
{
    HumanoidArticulatedBody* hab = new HumanoidArticulatedBody( argSim->getWorldID(),
                                                                argSim->getSpaceID(),
                                                                myHeight,
                                                                myWeight,
                                                                myHipBreadth );
    hab->setAnkleStrength( myAnkleStrength );
    hab->setFootLengthPerHeight( myFootLengthPerHeight );
    hab->setFootWidthPerHeight( myFootWidthPerHeight );

    return ArticulatedBodyPtr( hab );
}

Else::AcrobotArticulatedBodyFactory
::AcrobotArticulatedBodyFactory( double argScale )
    : myScale( argScale )
{
    // Noop
}

Else::ArticulatedBodyPtr
Else::AcrobotArticulatedBodyFactory
::create( PhysicsSimulatorPtr argSim )
{
    AcrobotArticulatedBody* ptr = new AcrobotArticulatedBody( argSim->getWorldID(),
                                                               argSim->getSpaceID(),
                                                               myScale );
    return ArticulatedBodyPtr( ptr );
}

