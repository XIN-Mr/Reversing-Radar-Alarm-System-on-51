#ifndef _CHAO_H_
#define _CHAO_H_

#include "common.h"

extern bit flag;
extern void startb();

extern void inittimer0();
extern void waitb();

sbit trigb=P2^0;
sbit echob=P2^1;

#endif
