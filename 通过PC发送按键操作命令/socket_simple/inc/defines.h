#ifndef _DEFINES_H
#define _DEFINES_H

typedef signed char         sint8;
typedef signed short        sint16;
typedef signed int          sint32;
typedef signed long         slng32;
typedef signed long long    sint64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long       ulng32;
typedef unsigned long long  uint64;

#define true    0
#define false   -1

#define U8TOU32(u24, u16, u8, u0)   ((((u24) & 0xFF) << 24) | (((u16) & 0xFF) << 16) | (((u8) & 0xFF) << 8) | ((u0) & 0xFF))
#define IP4(a)  ((a) & 0xFF)
#define IP3(a)  (((a) >> 8) & 0xFF)
#define IP2(a)  (((a) >> 16) & 0xFF)
#define IP1(a)  (((a) >> 24) & 0xFF)

#define BIT32(x)        (1 << (x))
#define BIT64(x)        (1ULL << (x))
#define SET_BIT32(x, n) ((x) | BIT32(n))
#define GET_BIT32(x, n) (((x) >> (n)) & 0x1)
#define CLR_BIT32(x, n) ((x) & ~BIT32(n))

#define UBYTESEL(x,b)               (((x) >> ((b) << 3)) & 0xFF)
#define PACKBYTES(high, low)        ((((high) << 8) | (low)) & 0xFFFF)
#define PACK16LSB(a, b)             ((((a) << 16) & 0xFFFF0000) | ((b) & 0x0000FFFF))
#define U8TOU32(u24, u16, u8, u0)   ((((u24) & 0xFF) << 24) | (((u16) & 0xFF) << 16) | (((u8) & 0xFF) << 8) | ((u0) & 0xFF))
#define BCD2DEC(x)                  (((x) >> 4) & 0xF) * 10 + ((x) & 0xF)
#define DEC2BCD(x)                  (((x) / 10) << 4) | ((x) % 10)
#define PACKCOLOR(r, g, b)          ((((b) >> 3) & 0x1F) | (((g) << 2) & 0x3E0) | (((r) << 7) & 0x7C00))
#define ALIGN_N(x, align)           (((x) + ((align) - 1)) & ~((align) - 1))
#define CLOSE_ALIGN_BIT32(x, bit)   MUX((x) & BIT32(bit - 1), ((x) + (BIT32(bit) - 1)) & ~(BIT32(bit) - 1), (x) & ~(BIT32(bit) - 1))

#define DBG
#ifdef  DBG
#define dbg(str, args...)		printf("\033[1;33m %s>: FUNCTION: %s, LINE: %d, \033[0m"str, __FILE__, __FUNCTION__, __LINE__, ## args);
#else
#define dbg(str, args...)
#endif
#define err(str, args...)		printf("\033[0;31m %s>: FUNCTION: %s, LINE: %d, \033[0m"str, __FILE__, __FUNCTION__, __LINE__, ## args);

#endif
