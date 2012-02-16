#ifndef ARTICULATEDBODYFACTORY_HPP
#define ARTICULATEDBODYFACTORY_HPP

#include "Else.hpp"

namespace Else
{
    /// Abstract Factory for creating ArticulatedBody's.  Subclasses hold all the needed
    /// data to create an ArticulatedBody of their type.
    class ArticulatedBodyFactory
    {
    public:
        ArticulatedBodyFactory() {}
        virtual ~ArticulatedBodyFactory() {}
        virtual ArticulatedBodyPtr create( PhysicsSimulatorPtr argSim ) = 0;
    };


    class HumanoidArticulatedBodyFactory : public ArticulatedBodyFactory
    {
    public:
        HumanoidArticulatedBodyFactory( double argHeight,
                                        double argWeight,
                                        double argHipBreadth,
                                        double argAnkleStrength,
                                        double argFootLengthPerHeight,
                                        double argFootWidthPerHeight );

        virtual ArticulatedBodyPtr create( PhysicsSimulatorPtr argSim );
    private:
        double myHeight;
        double myWeight;
        double myHipBreadth;
        double myAnkleStrength;
        double myFootLengthPerHeight;
        double myFootWidthPerHeight;
    };


    class AcrobotArticulatedBodyFactory : public ArticulatedBodyFactory
    {
    public:
        AcrobotArticulatedBodyFactory( double argScale );
        virtual ArticulatedBodyPtr create( PhysicsSimulatorPtr argSim );
    private:
        double myScale;
    };
}

#endif // ARTICULATEDBODYFACTORY_HPP
