#ifndef _MEC_H_
#define _MEC_H_

typedef unsigned char boolean;
typedef boolean u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define TRUE		1
#define FALSE		0

#define NOT_FOUND		-1

#define SET(v, i) (v |= (1 << ((i) ^ 7)))
#define CLR(v, i) (v &= ~(1 << ((i) ^ 7)))
#define	GET(v, i) (((v) & (1 << ((i) ^ 7))) != 0)

#endif
