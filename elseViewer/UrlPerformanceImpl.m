//
//  URLPerformance.m
//  elseViewer
//
//  Created by vector on 9/7/09.
//  Copyright 2009 Brian Allen (vector@acm.org) All rights reserved.
//

#import "URLPerformanceImpl.h"


@implementation UrlPerformanceImpl

- (UrlPerformanceImpl*) initWithId: (long int) performanceId
{
    self = [super init];
    if( self )
    {
        [self loadById:performanceId];
    }
    return self;
}

- (void) dealloc
{
    [_data release];
    [super dealloc];
}
- (void) loadById:(long int)performanceId
{

    NSURL* url = [NSURL URLWithString:[[NSString alloc] initWithFormat:@"http://evocomp.org/else/performance.php?performance_id=%d", performanceId]];
    if( url == nil )
    {
        NSLog(@"ERROR");
        return;
    }
    _data = [NSString stringWithContentsOfURL:url];

    url = nil;
    // TODO -- load timestep from performance metadata
}

- (int) length
{
    return [_data length];
}

- (void) getCString:(char *)buffer maxLength:(int)maxBufferSize
{
    [_data getCString:buffer maxLength:maxBufferSize encoding:NSASCIIStringEncoding];
}

- (double) timeStep
{
    return 0.0165; // TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
@end
