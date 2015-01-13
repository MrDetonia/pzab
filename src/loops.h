/*
 *  Copyright (C) 2015 Zac Herd
 *  See LICENSE for more information
 */

// linked list structure used to track loops in code
struct item
{
    struct item* prev;

    unsigned char count;
    long inst;
}* loops;

// push adds a new loop to the list, setting the loop count and instruction
void push(const unsigned char count, const long inst);

// pops a loop from the list
void pop();
