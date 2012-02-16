//
//  AnimController.m
//  elseViewer
//
//  Created by vector on 8/2/09.
//  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
//

#import "AnimController.h"
#include <dispatch/dispatch.h>

#include <vector>
float sceneRotation[] = { -90.0, 1.0, 0.0, 0.0 };
//{ -90.0, 1.0, 0.0, 0.0 };

/////////////////////////////////////

@implementation AnimController

- (id) init
{
    [super init];
    _impl = new Else::AnimControllerImpl;
    _isLoading = NO;
    
    // Queue up a request to load the initial performance
    //[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(loadPerformance) userInfo:nil repeats:NO];    
    return self;
}

- (IBAction) loadPerformance
{
    NSLog( @"AnimController:loadPerformance" );
    _isLoading = YES;
    
    NSInteger pid = [_performanceIdTextField integerValue];
    if( pid < 1 )
    {
        return;
    }
    [_view logMsg:[NSString stringWithFormat:@"Loading performance id %d...", pid] ]; 
    
    assert( _impl );
    
    dispatch_async( dispatch_get_global_queue(0, 0), ^{
        _impl->loadPerformance( pid );
        dispatch_async( dispatch_get_main_queue(), ^{
            [_view logMsg:[NSString stringWithFormat:@"Loading complete. [%d]", pid] ]; 
            [_view setAnimate:YES];
            [_frameSlider setMaxValue: _impl->getStates().size() - 1];
        });
    });
    _isLoading = NO;
    
}

- (IBAction) loadPerformanceAction:(id)sender
{
   [self loadPerformance];
}

-(IBAction) setPerformanceId:(int)pid
{
    NSLog( @"AnimController:setPerformanceId:%d", pid );
    [_performanceIdTextField setIntegerValue:pid];
    [self loadPerformance];
}
     
-(void) setStateId:(int)sid
{
    //NSLog( @"AnimController:setStateId:%d", sid );
    [_stateIdTextField setIntegerValue:sid];
    _impl->setStateId( sid );
    [_view setNeedsDisplay:YES];
}

-(int) stateId
{
    return (int)_impl->getStateId();
}

- (IBAction) draw
{
    glPushMatrix();
    glRotatef (sceneRotation[0], sceneRotation[1], sceneRotation[2], sceneRotation[3]);
    _impl->draw();
    glPopMatrix();
}

- (IBAction) update:(double)dt
{
    [self willChangeValueForKey:@"stateId"];
    if( !_isLoading )
    {
        _impl->update( dt );
    }
    [self didChangeValueForKey:@"stateId"];
}

- (IBAction) update
{
    if( !_isLoading )
    {
        _impl->update();
    }
}

// ---------------------------------


- (IBAction) togglePlayback:(id) sender
{
    
}

@end
