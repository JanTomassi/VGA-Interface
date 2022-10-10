#ifndef __PROGRAMMES_H
#define __PROGRAMMES_H

#define PRO_NUM_PROGRAM 5

typedef struct program
{
    void (*program)();
    const char * name;
} program;

program programmes[PRO_NUM_PROGRAM];


#endif