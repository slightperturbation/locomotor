//
//  URLPerformanceImpl.h
//  elseViewer
//
//  Created by vector on 9/7/09.
//  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
//

#import <Cocoa/Cocoa.h>

/// Simple Objective-C class for querying the DB via PHP middlelayer.
@interface UrlPerformanceImpl : NSObject 
{
    NSString* _data;
    double    _timeStep;
    long int  _performanceId;
}

- (UrlPerformanceImpl*) initWithId: (long int) peformanceId;
- (void)                dealloc;
- (void)                loadById:   (long int) performanceId;
- (int)                 length;
- (void)                getCString: (char*) buffer maxLength: (int)maxBufferSize;
- (double)              timeStep;

@end
