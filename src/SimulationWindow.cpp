#include <SimulationWindow.hpp>
#include <RibRenderer.hpp>

#include <EvolverUI.hpp>

void 
Else::SimulationWindow
::draw() 
{	// the draw method must be private
    if( !myIsEnabled || !mySim || !mySim->getBody() ) {
        return;
    }

    EvolverUI* p = (EvolverUI*)( parent()->parent()->parent()->user_data() );
    if( p != NULL )
    {
        p->showActuators( mySim->getBody()->getActuatorActivation() );
    }

    myRenderer->resize( w(), h() );

    glColor3f( 0.3f, 0.3f, 0.6f ); 

    myRenderer->render( mySim->getBody() );

    static Renderables statics;
    mySim->getStatics( statics );

    for( Renderables::iterator i = statics.begin(); i != statics.end(); ++i )
    {
        myRenderer->render( *i );
    }
    statics.clear();

    window()->redraw();

    // post update
    myRenderer->flush();
}