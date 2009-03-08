/******************************************************************************/
// Bullfrog Engine Emulation Library - for use to remake classic games like
// Syndicate Wars, Magic Carpet or Dungeon Keeper.
/******************************************************************************/
/** @file bflib_memory.h
 *     Header file for bflib_memory.c.
 * @par Purpose:
 *     Memory managing routines.
 * @par Comment:
 *     Just a header file - #defines, typedefs, function prototypes etc.
 * @author   Tomasz Lis
 * @date     10 Feb 2008 - 30 Dec 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#ifndef BFLIB_MEMORY_H
#define BFLIB_MEMORY_H

#include "bflib_basics.h"
#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
typedef unsigned char * __fastcall MemAllocFunc(ulong);
/******************************************************************************/
DLLIMPORT extern unsigned long _DK_mem_size;
#define mem_size _DK_mem_size
/******************************************************************************/
short update_memory_constraits(void);
void * __fastcall LbMemorySet(void *dst, uchar c, ulong length);
void * __fastcall LbMemoryCopy(void *in_dst, const void *in_src, ulong len);
int __fastcall LbMemorySetup();
short LbMemoryCheck(void);
unsigned char * __fastcall LbMemoryAllocLow(ulong size);
unsigned char * __fastcall LbMemoryAlloc(ulong size);
int __fastcall LbMemoryFree(void *mem_ptr);
void * __fastcall LbStringCopy(char *dst, const char *src, const ulong dst_buflen);
void * __fastcall LbStringConcat(char *dst, const char *src, const ulong dst_buflen);
ulong __fastcall LbStringLength(const char *str);
/******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
