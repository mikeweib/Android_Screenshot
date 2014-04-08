//
//  AppDelegate.h
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-10.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (assign) IBOutlet NSButton *button;

- (IBAction)onButton:(id)sender;

+ (void)savePNG:(char*) buffer height:(int)height width:(int)width path:(char*)path;

@end
