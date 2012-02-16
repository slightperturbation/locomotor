/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef ELSE_FITNESSGRAPH_HPP
#define ELSE_FITNESSGRAPH_HPP
//
// $Id: FitnessGraph.hpp,v 1.4 2004/07/19 00:30:12 dexabraxis Exp $
// $Date: 2004/07/19 00:30:12 $
//
// Copyright © 2004, Dexter Allen
//
// Contact: vector@acm.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


// Project Includes
#include <Else.hpp>
#include <FitnessFunc.hpp>

// External Includes
#include <Fl/fl_draw.H>
#include <Fl/Enumerations.H>
#include <gmtl/Math.h>

namespace Else 
{
    class FitnessGraph : public Fl_Widget
    {
    public:
        FitnessGraph( int x, int y, int w, int h, const char *label = 0 )
            : Fl_Widget( x, y, w, h, label ),
            myFitnessFunc( (FitnessFunc*)0 ),
            myMaxFitness( 105.0 ),
            myGenerations( 250 ),
            myBestFitness( 0 )
        {
        }

        void setFitness( FitnessFuncPtr argFitnessVector )
        {
            myFitnessFunc = argFitnessVector;
            redraw();
        }

        void setTotalGenerations( int argGenerations )
        {
            myGenerations = argGenerations;
        }

        void setMaxFitness( int argFitness )
        {
            myMaxFitness = argFitness;
        }

    protected:
        void draw( void )
        {
            static int call = 0;
            if( !( ++call % 100) )
            {
                // don't draw so much
                return;
            }
            // draw zero-line
            fl_color( FL_DARK_YELLOW );
            fl_line( x(), y() + h() * .8, x() + w(), y() + h() * .8 );

            fl_color( FL_DARK_RED );
            fl_line( x(), fitnessToPixels( myBestFitness ), 
                x() + w(), fitnessToPixels( myBestFitness ) );
            fl_color( FL_BLACK );
            if( myFitnessFunc )
            {
                std::vector<double>& fitVec = myFitnessFunc->getHistory();
                float pixelsPerTick = (float)w() / (float)myGenerations;

                fl_begin_line();

                for( int i = 0; i < fitVec.size(); ++i )
                {
                    float pixelX = x() + i * pixelsPerTick;
                    fl_vertex( pixelX, fitnessToPixels( fitVec[i] ) );
                    myBestFitness = std::max<double>( myBestFitness, fitVec[i] );
                }
                fl_end_line();
            }
        }

        float fitnessToPixels( double argFitness )
        {
            float pixelsPerFitness = (float)h() / (float)myMaxFitness;
            float pixelY = (argFitness - 100.0) * pixelsPerFitness ;
            pixelY /= 1.4;
            pixelY += h() * .2;
            pixelY = y() + h() - pixelY;
            return pixelY;
        }

    private:
        double myBestFitness;

        FitnessFuncPtr myFitnessFunc;
        int myGenerations;
        double myMaxFitness;
    };
}

#endif
//////////////////////////////////////////////////////////////////////////
// Historical Log:
//
// $Log: FitnessGraph.hpp,v $
// Revision 1.4  2004/07/19 00:30:12  dexabraxis
// Many bug fixes and better harness and Echo state net support.
//
// Revision 1.3  2004/06/28 00:08:05  dexabraxis
// Last good checkin before echo state networks
//
// Revision 1.2  2004/05/14 01:01:07  dexabraxis
// *** empty log message ***
//
// Revision 1.1  2004/05/12 09:43:48  dexabraxis
// Tweaks to the fitness function, Started on fitness graph.
//
//
//////////////////////////////////////////////////////////////////////////

