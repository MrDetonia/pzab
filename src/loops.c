/*
 *  Copyright (C) 2015 Zac Herd
 *  See LICENSE for more information
 */

#include "loops.h"

void push(const unsigned char count, const long inst)
{
    struct item newLoop;
    newLoop.count = count;
    newLoop.inst = inst;
    newLoop.prev = loops;
    loops = &newLoop;
}

void pop()
{
    loops = loops->prev;
}
