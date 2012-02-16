//
//  AnimController.h
//  elseViewer
//
//  Created by vector on 8/2/09.
//  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "AnimControllerImpl.hpp"
#import "BasicOpenGLView.h"

//namespace Else
//{
//    class PhysicsSimulator;
//    class ArticulatedBody;
//    class GLRenderer;
//}

@class BasicOpenGLView;
class Else::AnimControllerImpl;

@interface AnimController : NSObject 
{
    IBOutlet NSButton* _togglePlayButton;
    IBOutlet NSTextField* _performanceIdTextField;
    IBOutlet NSTextField* _stateIdTextField;
    IBOutlet NSSlider* _frameSlider;
    IBOutlet BasicOpenGLView* _view;
    //void* _scene;
    BOOL _isLoading;
    
    Else::AnimControllerImpl* _impl;
    
    /// TEMPORARY
	// spin
	GLfloat objectRotation [4];
	GLfloat rRot [3];
	GLfloat rVel [3];
	GLfloat rAccel [3];
}
- (id)       init;
- (IBAction) draw;
- (IBAction) update;
- (IBAction) update:(double)dt;
- (IBAction) togglePlayback:(id)sender;
- (IBAction) loadPerformance;
- (IBAction) loadPerformanceAction:(id)sender;
- (IBAction) setPerformanceId:(int)pid;
- (void)     setStateId:(int)sid;
- (int)      stateId;

@end
