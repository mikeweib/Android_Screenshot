//
//  TestC11.h
//  AndroidScreenShot
//
//  Created by Wei Mike on 14-1-17.
//  Copyright (c) 2014年 Liqu. All rights reserved.
//

#ifndef __AndroidScreenShot__TestC11__
#define __AndroidScreenShot__TestC11__

class BaseClass {
public:
    virtual void test(int i);
};

class SubClass : public BaseClass {
public:
    virtual void test(double i);
};

class Sub2Class : public SubClass {
public:
    virtual void test(double i) override;
    virtual void test(int i) override;
};

#endif /* defined(__AndroidScreenShot__TestC11__) */
