//
//  AppDelegate.m
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-10.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#import "AppDelegate.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import <QTKit/QTKit.h>
#include "AdbHelper.h"
#include "TestC11.h"

class fbCallback : public RawImageCallback {
    
    void onGetFrameBuffer(SCREENSHOT_RET_CODE ret, RawImage* rawImage) {
        char path[256] = "/Users/weimike/android_screen_capture.png";
        ggLogD("onGetFrameBuffer");
        if (ret == SCREENSHOT_GET_IMAGE_SUCCESS) {
            [AppDelegate savePNG:rawImage->data height:rawImage->height width:rawImage->width path:path];
        }
    }
    
};


@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    AdbHelper::getInstance()->setCallback(new fbCallback());
    // Insert code here to initialize your application
}

- (IBAction)onButton:(id)sender {
    SubClass* t = new SubClass();
    t->test(1);
    delete t;
    
    // async called
    AdbHelper::getInstance()->asyncGetFrameBuffer("127.0.0.1", 5037, "30315F6D9BCD00EC");
    
    // sync called.
//    char path[256] = "/Users/weimike/android_screen_capture1.png";
//    RawImage rawImage;
//    SCREENSHOT_RET_CODE ret = AdbHelper::getInstance()->getFrameBuffer("127.0.0.1", 5037, "30315F6D9BCD00EC", rawImage);
//    if (ret == GET_IMAGE_SUCCESS) {
//        [AppDelegate savePNG:rawImage.data height:rawImage.height width:rawImage.width path:path];
//    }
}

+ (void)savePNG:(char*) buffer height:(int)height width:(int)width path:(char*)path {
    int length = width * height * sizeof(int);
    // make data provider with data.
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, length, NULL);
	
    // prep the ingredients
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    int bytesPerRow = 4 * width;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
    // make the cgimage
    CGImageRef imageRef = CGImageCreate(width,
										height,
										bitsPerComponent,
										bitsPerPixel,
										bytesPerRow,
										colorSpaceRef,
										bitmapInfo,
										provider,
										NULL,
										NO,
										renderingIntent);
	
    // then create a bitmap representation from CGImage
	NSBitmapImageRep* rep = [[[NSBitmapImageRep alloc] initWithCGImage:imageRef] autorelease];
	
	// save to path
	NSData* data = [rep representationUsingType:NSPNGFileType
									 properties:[NSDictionary dictionary]];
	NSString* p = [NSString stringWithCString:path encoding:NSUTF8StringEncoding];
	NSFileManager* fm = [NSFileManager defaultManager];
	[fm createDirectoryAtPath:[p stringByDeletingLastPathComponent]
  withIntermediateDirectories:YES
				   attributes:nil
						error:NULL];
	[data writeToFile:p atomically:YES];
	
	// release
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider);
	CGImageRelease(imageRef);
}
@end
