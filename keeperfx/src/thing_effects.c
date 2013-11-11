/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file thing_effects.c
 *     Effect generators and effect elements support functions.
 * @par Purpose:
 *     Functions to create and maintain effect generators and single effect elements.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     01 Jan 2010 - 01 Feb 2012
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "thing_effects.h"
#include "globals.h"

#include "bflib_memory.h"
#include "bflib_math.h"
#include "bflib_sound.h"
#include "bflib_planar.h"

#include "thing_objects.h"
#include "thing_list.h"
#include "thing_stats.h"
#include "thing_physics.h"
#include "thing_navigate.h"
#include "creature_senses.h"
#include "front_simple.h"
#include "map_data.h"
#include "map_blocks.h"
#include "creature_graphics.h"
#include "gui_topmsg.h"
#include "game_legacy.h"
#include "engine_redraw.h"
#include "keeperfx.hpp"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
DLLIMPORT struct Thing *_DK_create_effect_element(const struct Coord3d *pos, unsigned short a2, unsigned short a3);
DLLIMPORT struct Thing *_DK_create_effect_generator(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4);
DLLIMPORT void _DK_poison_cloud_affecting_area(struct Thing *thing, struct Coord3d *pos, long a3, long a4, unsigned char a5);
DLLIMPORT void _DK_process_spells_affected_by_effect_elements(struct Thing *thing);
DLLIMPORT long _DK_update_effect_element(struct Thing *thing);
DLLIMPORT long _DK_update_effect(struct Thing *thing);
DLLIMPORT long _DK_process_effect_generator(struct Thing *thing);
DLLIMPORT struct Thing *_DK_create_effect(const struct Coord3d *pos, unsigned short a2, unsigned char a3);
DLLIMPORT long _DK_move_effect(struct Thing *thing);
DLLIMPORT long _DK_move_effect_element(struct Thing *thing);
DLLIMPORT void _DK_change_effect_element_into_another(struct Thing *thing, long nmodel);
DLLIMPORT void _DK_explosion_affecting_area(struct Thing *thing, const struct Coord3d *pos, long a3, long a4, unsigned char a5);

/******************************************************************************/
extern struct EffectElementStats _DK_effect_element_stats[95];
//DLLIMPORT struct InitEffect _DK_effect_info[];
//#define effect_info _DK_effect_info
//extern struct EffectGeneratorStats _DK_effect_generator_stats[6];
//#define effect_element_stats _DK_effect_element_stats
/******************************************************************************/
struct EffectGeneratorStats effect_generator_stats[] = {
    { 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    {10, 20,  1, 30, 1,  0,-40, 40,-40, 40, 80,150,147,  3, 0},
    {10, 20,  1, 31, 0, -1,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 0,  0,  5, 33, 0, -1,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 0,  2,  1, 37, 0,256,-15, 15,-15, 15,  0,  0,  0,  0, 0},
    { 2,  5,  1, 37, 0,  0,-15, 15,-15, 15,  0,  0,  0,  0, 0}
};

struct InitEffect effect_info[] = {
    { 0, 1,   0,   0,  0,    0,  0,   0,  0,  0,  0, 0, {0}, 0},
    { 1, 1,  32,  32, -32,  32,  1,  47,  1,  1,  0, 1, { 512, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 5, 1,  32,  32, -64,  64,  5,  47,  1,  1,  0, 1, {1024, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    {10, 1, 128, 128,-128, 128, 10,  47,  1,  1,  0, 1, {2048, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    {10, 1, 172, 172,-172, 172,  6,  47,  1,  1,  0, 1, {2560, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    {20, 1, 256, 256,-256, 256, 10,  47,  1,  1,  0, 1, {2560, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 1, 1,  32,  32, -96,  96,  2,   0, 84, 84,  0, 1, {0}, 1},
    { 2, 1,  32,  32, -96,  96,  2,   0, 84, 84,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  4,   0, 84, 84,  0, 1, {0}, 1},
    { 3, 1,  96,  96, -96,  96,  4,   0, 84, 84,  0, 1, {0}, 1},
    { 4, 1,  96,  96, -96,  96,  5,   0, 84, 84,  0, 1, {0}, 1}, // [10]
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  1, 1, {0}, 1},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  1, 1, {0}, 1},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  1, 1, {0}, 1},
    {10, 1, 100, 100,   1,   1, 20, 178, 10, 10,  4, 1, {2560, 52, 0, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 1, 1,   1,   1,   1,   1,  1,   0, 11, 11,  0, 1, {0}, 1},
    {40, 1,  64,  64, -64,  64,  2,  52, 21, 21,  0, 1, {0}, 1},
    {40, 1,  64,  64, -64,  64,  2,  52, 21, 21,  0, 1, {0}, 1},
    {40, 1,  64,  64, -64,  64,  2,  52, 21, 21,  0, 1, {0}, 1},
    { 1, 1,   1,   1,   1,   1,  1,   0, 22, 22,  0, 1, {0}, 1},
    { 1, 1,   1,   1,   1,   1,  1,   0, 22, 22,  0, 1, {0}, 1}, // [20]
    { 1, 1,   1,   1,   1,   1,  1,   0, 22, 22,  0, 1, {0}, 1},
    { 1, 1,  32,  32, -96,  96,  1,   0, 24, 24,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  4,   0, 24, 24,  0, 1, {0}, 1},
    { 3, 1, 128, 128, -96,  96,  4,   0, 24, 24,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  2,   0, 26, 26,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  1,   0, 27, 28,  0, 1, {0}, 1},
    { 3, 1,  64,  64, -96,  96, 10,   0, 26, 28,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 75, 75,  0, 1, {0}, 1},
    { 1, 1,   1,   1,   1,   1,  1,   0, 40, 40,  0, 1, {0}, 1},
    {80, 2,   1,   1,   1,   1,  1,   0, 21, 21,  0, 0, {0}, 1},
    { 8, 1,  64,  64, -64,  64,  1,   0, 47, 47,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  2,   0, 49, 49,  0, 1, {0}, 1},
    { 2, 1,  64,  64, -96,  96,  1,   0, 49, 51,  0, 1, {0}, 1},
    { 3, 1,  64,  64, -96,  96, 10,   0, 50, 51,  0, 1, {0}, 1},
    { 8, 1,  16,  16, -16,  16,  1,   0, 29, 29,  0, 1, {0}, 0},
    {32, 1,  32,  32, -32,  32,  2,   0, 26, 28,  0, 1, {0}, 0},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  2, 1, {0}, 1},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  2, 1, {0}, 1},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  2, 1, {0}, 1},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  3, 1, {0}, 0},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  3, 1, {0}, 0},
    {40, 1,  44,  44, -32,  32,  2,  52,  7,  7,  3, 1, {0}, 0},
    {16, 1, 128, 128,-128, 128,  2,  47, 26, 32,  0, 1, {2560, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 0},
    { 1, 1,  64,  64,-128, 128,  4,   0, 53, 53,  0, 1, {0}, 0},
    {16, 1,  96,  96, -96,  96,  4,  47, 39, 39,  1, 1, {2560, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 5, 1,  64,  64, -64,  64,  4,  39, 75, 75,  0, 1, { 768, 20, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    {60, 3,   1,   1,   1,   1,  2,  54, 55, 58,  0, 1, {0}, 1},
    {20, 4,   1,   1,   1,   1,  1,  47,  0,  0,  0, 1, {0}, 1},
    {50, 4,   1,   1,   1,   1,  1,   0,  0,  0,  0, 0, {0}, 0},
    {10, 1, 128, 128,-128, 128, 10,  47,  1,  1,  0, 1, {4096, 50, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 1, 1,   1,   1,   1,   1,  1, 112, 61, 61,  0, 1, {0}, 1},
    { 5, 1, 128, 128,-128, 128,  5,  47,  1,  1,  0, 1, {2048, 45, 1, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    {96, 1, 256, 256,-256, 256,  1, 160, 63, 74,  0, 1, {0}, 0},
    { 8, 1,  64,  64, -64,  64,  1, 159, 63, 66,  0, 1, {0}, 0},
    { 8, 1,  64,  64, -64,  64,  1, 159, 67, 70,  0, 1, {0}, 0},
    { 8, 1,  64,  64, -64,  64,  1, 159, 71, 74,  0, 1, {0}, 0},
    { 4, 1,  16,  16, -32,  64,  3,   0, 76, 76,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 77, 77,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 78, 78,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 54, 54,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 79, 79,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 80, 80,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 81, 81,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 82, 82,  0, 1, {0}, 1},
    { 1, 1,  32,  32, 100, 100,  2,   0, 84, 84,  0, 1, {0}, 1},
    { 1, 1,   1,   1,   1,   1,  2,   0, 85, 85,  0, 1, {0}, 1},
    { 4, 1,  16,  16, -32,  64,  3,   0, 75, 78,  0, 1, {0}, 1},
    {10, 1,  20, 150, -80,  80, 20,  36, 27, 29, 29, 1, {2560, 52, 0, 0, 0, 0, {{0},{0},{0}}, 0, 0, 0}, 1},
    { 0, 0,   0,   0,   0,   0,  0,   0,  0,  0,  0, 0, {0}, 0},
};


struct EffectElementStats effect_element_stats[] = {
   // [0]
   {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 926, 152, 192, 1, 192, 256, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0, 30, 40, 96, 122, 142, 0, 192, 256, 0,
    0, 0, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1,
    0, 0, 256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0, 30, 40, 96, 122, 142, 0, 192, 256, 0,
    0, 0, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1,
    0, 0, 256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0, 30, 40, 96, 122, 142, 0, 192, 256, 0,
    0, 0, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1,
    0, 0, 256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1},
    // [5]
   {2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, 5, 5, 916, 110, 128, 0, 256, 256, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, 30, 40, 929, 320, 374, 0, 128, 128, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0, 20, 20, 981, 172, 192, 0, 256, 256, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0},
   {2, 2, 0, -1, -1, 927, 50, 256, 1, 256, 320, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // [10]
   {2, 1, 0,    -1,    -1, 798, 50, 256, 1, 256, 320, 1, 1,
    3, 0, 0, 2, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256,
    0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1},
   {2, 1, 0,    15,    15, 908, 256, 256, 1, 256, 256, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0,    15,    15, 913, 226, 256, 0, 256, 256, 0, 1,
    0, 1, 0, 0, 8, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    15,    15, 914, 226, 256, 0, 256, 256, 0, 1,
    0, 1, 0, 0, 8, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    15,    15, 915, 226, 256, 0, 256, 256, 0, 1,
    0, 1, 0, 0, 8, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
    // [15]
   {2, 5, 0,     1,     1, 964, 24, 24, 0, 256, 256, 1, 1, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0,    -1,    -1, 918, 96, 96, 0, 256, 256, 1, 1,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0,     1,     1, 917, 96, 96, 0, 256, 256, 1, 1, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0,     6,     6, 964, 64, 64, 0, 256, 256, 1, 1, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0,     1,     1, 981, 192, 192, 0, 256, 256, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // [20]
   {2, 5, 0, 20000, 20000, 907, 192, 192, 0, 256, 256,
    1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0,    40,    40, 919, 320, 374, 0, 128, 128, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0,    -1,    -1, 909, 250, 300, 1, 128, 128, 1, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0,    -1,    -1, 836, 200, 256, 1, 16, 32, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0,    40,    50, 910, 150, 180, 0, 256, 256, 0, 0,
    2, 1, 0, 0, 8, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 25, 0, 0, 0, 0, 0, 0, 0, 1},
    // [25]
   {2, 1, 0,    -1,    -1, 911, 150, 180, 1, 16, 32, 1, 0,
    2, 1, 0, 0, 8, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    30,    40, 834, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    30,    40, 833, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    30,    40, 832, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0,    -1,    -1, 828, 250, 300, 1, 128, 128, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // [30]
   {2, 1, 0,    30,    40, 823, 122, 142, 0, 192, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 16, 0, 0, 0, 32, 1, 1, 0, 0,
    256, 0, 19, 36, 80, 1, 0, 0, 256, 1, 0, 1280, 52, 0,
    0, 0, 0, 0, 1},
   {2, 1, 0,    30,    40, 827, 122, 142, 0, 192, 256, 0, 0,
    2, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1, 19, 36,
    80, 1, 19, 36, 80, 1, 0, 0, 256, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 5, 0,    -1,    -1, 824, 122, 142, 1, 192, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 1, 0,    10,    15, 832, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 5, 0,    -1,    -1, 0, 450, 450, 1, 256, 256, 1, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0},
    // [35]
   {2, 5, 0, 20000, 20000, 0, 450, 450, 0, 256, 256,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 5, 0, -1, -1, 0, 450, 450, 1, 256, 256, 1, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 110, 225, 270, 1, 85, 85, 1, 0,
    2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1},
   {2, 5, 0, -1, -1, 825, 256, 256, 1, 256, 256, 1, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1},
   {2, 1, 0, -1, -1, 926, 122, 192, 0, 192, 256, 1, 0,
    1, 0, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
    // [40]
   {2, 5, 0, -1, -1, 828, 250, 300, 1, 128, 128, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {4, 4, 0, 16, 16, 0, 256, 256, 0, 256, 256, 1, 1, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, 2, 4, 964, 128, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 852, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, 4, 4, 852, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 802, 172, 196, 0, 256, 256, 1, 1,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, 100, 100, -1, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0, 20, 30, 919, 320, 374, 0, 128, 128, 1, 1,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 1, 0, 10, 15, 832, 122, 142, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0, 30, 40, 831, 130, 180, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0, 30, 40, 831, 180, 250, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 1, 0, 30, 40, 831, 250, 350, 0, 192, 256, 0, 0,
    0, 1, 0, 0, 40, 0, 102, 0, 0, 0, 0, 0, 1, 0, 0,
    256, 0, 19, 36, 80, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 5, 0, 1, 1, 964, 64, 64, 0, 256, 256, 1, 1, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, 6, 12, 964, 64, 96, 0, 256, 256, 1, 1, 3,
    0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0,
    0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 856, 128, 128, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 863, 320, 374, 1, 256, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 5, 0, -1, -1, 864, 320, 374, 1, 256, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 5, 0, -1, -1, 865, 320, 374, 1, 256, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 5, 0, -1, -1, 866, 320, 374, 1, 256, 256, 0, 0,
    3, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0},
   {2, 5, 0, 8, 8, 819, 256, 256, 1, 256, 256, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, 5, 5, 964, 96, 160, 1, 85, 85, 1, 1, 3, 0,
    0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 120, 256, 256, 1, 256, 256, 1, 1,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0, 1, 1, -1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 2, 0, -1, -1, 116, 256, 256, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 117, 256, 256, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 118, 256, 256, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 119, 256, 256, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 116, 358, 358, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 117, 358, 358, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 118, 358, 358, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 119, 358, 358, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 116, 460, 460, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 117, 460, 460, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 118, 460, 460, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 2, 0, -1, -1, 119, 460, 460, 0, 128, 128, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 966, 172, 255, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 967, 172, 255, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 968, 172, 255, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 969, 172, 255, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 857, 128, 128, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 858, 128, 128, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 859, 128, 128, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 860, 128, 128, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 852, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 1, 30, 40, 96, 160, 256, 0, 192, 256, 0, 0,
    1, 1, 0, 0, 10, 0, 102, 0, 0, 0, 0, 0, 1, 66, 0,
    256, 1, 66, 36, 100, 1, 0, 0, 256, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1},
   {2, 5, 0, -1, -1, 97, 250, 300, 1, 128, 128, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0, -1, -1, 853, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 854, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 855, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, -1, -1, 110, 225, 270, 1, 1024, 1024, 1, 0,
    2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
   {2, 5, 0, -1, -1, 853, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 854, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 855, 172, 172, 1, 256, 256, 1, 1,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 1, 0, 5, 5, 917, 96, 160, 1, 85, 85, 1, 1, 3, 0,
    0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0,
    256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {2, 5, 0, -1, -1, 837, 200, 256, 1, 16, 32, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0,
    0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

long const bounce_table[] = { -160, -160, -120, -120, -80, -40, -20, 0, 20, 40, 80, 120, 120, 160, 160, 160 };
/******************************************************************************/
struct InitEffect *get_effect_info(ThingModel effmodel)
{
    return &effect_info[effmodel];
}

struct InitEffect *get_effect_info_for_thing(const struct Thing *thing)
{
    return &effect_info[thing->model];
}

struct Thing *create_effect_element(const struct Coord3d *pos, unsigned short eelmodel, unsigned short owner)
{
    struct InitLight ilght;
    struct EffectElementStats *eestat;
    struct Thing *thing;
    long i,n;
    //return _DK_create_effect_element(pos, eelmodel, owner);
    if (!i_can_allocate_free_thing_structure(FTAF_Default)) {
        return INVALID_THING;
    }
    if (!any_player_close_enough_to_see(pos)) {
        return INVALID_THING;
    }
    eestat = &effect_element_stats[eelmodel];
    LbMemorySet(&ilght, 0, sizeof(struct InitLight));
    thing = allocate_free_thing_structure(FTAF_Default);
    if (thing->index == 0) {
        ERRORDBG(8,"Should be able to allocate effect element %d for player %d, but failed.",(int)eelmodel,(int)owner);
        return INVALID_THING;
    }
    thing->class_id = TCls_EffectElem;
    thing->model = eelmodel;
    thing->mappos.x.val = pos->x.val;
    thing->mappos.y.val = pos->y.val;
    thing->mappos.z.val = pos->z.val;
    thing->next_on_mapblk = 0;
    thing->parent_idx = thing->index;
    thing->owner = owner;
    thing->sizexy = 1;
    thing->field_58 = 1;
    thing->field_5A = 1;
    thing->field_5C = 1;

    if (eestat->sprite_idx != -1)
    {
        i = ACTION_RANDOM(eestat->sprite_size_max  - (int)eestat->sprite_size_min  + 1);
        n = ACTION_RANDOM(eestat->sprite_speed_max - (int)eestat->sprite_speed_min + 1);
        set_thing_draw(thing, eestat->sprite_idx, eestat->sprite_speed_min + n, eestat->sprite_size_min + i, 0, 0, eestat->field_0);
        set_flag_byte(&thing->field_4F,0x02,eestat->field_13);
        thing->field_4F ^= (thing->field_4F ^ (0x10 * eestat->field_14)) & 0x30;
        set_flag_byte(&thing->field_4F,0x40,eestat->field_D);
    } else
    {
        set_flag_byte(&thing->field_4F,0x01,true);
    }

    thing->field_20 = eestat->field_18;
    thing->field_23 = eestat->field_1A;
    thing->field_24 = eestat->field_1C;
    thing->movement_flags |= TMvF_Unknown08;
    set_flag_byte(&thing->movement_flags,TMvF_Unknown10,eestat->field_16);
    thing->creation_turn = game.play_gameturn;

    if (eestat->numfield_3 > 0)
    {
        i = ACTION_RANDOM(eestat->numfield_5 - (long)eestat->numfield_3 + 1);
        thing->health = eestat->numfield_3 + i;
    } else
    {
        thing->health = get_lifespan_of_animation(thing->field_44, thing->field_3E);
    }

    if (eestat->field_17 != 0)
    {
        thing->field_4B = eestat->sprite_size_min;
        thing->field_4D = eestat->sprite_size_max;
        if (eestat->field_17 == 2)
        {
            thing->field_4A = 2 * (eestat->sprite_size_max - (long)eestat->sprite_size_min) / thing->health;
            thing->field_50 |= 0x02;
        }
        else
        {
            thing->field_4A = (eestat->sprite_size_max - (long)eestat->sprite_size_min) / thing->health;
            thing->field_50 &= ~0x02;
        }
        thing->field_46 = eestat->sprite_size_min;
    } else
    {
        thing->field_4A = 0;
    }

    if (eestat->field_3A != 0)
    {
        ilght.mappos.x.val = thing->mappos.x.val;
        ilght.mappos.y.val = thing->mappos.y.val;
        ilght.mappos.z.val = thing->mappos.z.val;
        ilght.field_0 = eestat->field_3A;
        ilght.field_2 = eestat->field_3C;
        ilght.is_dynamic = 1;
        ilght.field_3 = eestat->field_3D;
        thing->light_id = light_create_light(&ilght);
        if (thing->light_id <= 0) {
            SYNCDBG(8,"Cannot allocate dynamic light to %s.",thing_model_name(thing));
        }
    }
    add_thing_to_list(thing, &game.thing_lists[TngList_EffectElems]);
    place_thing_in_mapwho(thing);
    return thing;
}

void process_spells_affected_by_effect_elements(struct Thing *thing)
{
  _DK_process_spells_affected_by_effect_elements(thing);
}

void move_effect_blocked(struct Thing *thing, struct Coord3d *prev_pos, struct Coord3d *next_pos)
{
    struct EffectElementStats *effstat;
    long cube_id,sample_id;
    unsigned short effmodel;
    unsigned long blocked_flags;
    effstat = &effect_element_stats[thing->model];
    blocked_flags = get_thing_blocked_flags_at(thing, next_pos);
    slide_thing_against_wall_at(thing, next_pos, blocked_flags);
    if ( ((blocked_flags & SlbBloF_WalledZ) != 0) && effstat->field_15 && effstat->field_22 )
    {
        struct Thing *efftng;
        efftng = thing;
        cube_id = get_top_cube_at(next_pos->x.stl.num, next_pos->y.stl.num);
        if (cube_is_water(cube_id))
        {
          effmodel = effstat->field_2A;
          if (effmodel > 0) {
              efftng = create_effect(prev_pos, effmodel, thing->owner);
              TRACE_THING(efftng);
          }
          sample_id = effstat->field_2C;
          if (sample_id > 0) {
              thing_play_sample(efftng, sample_id, 100, 0, 3, 0, 2, effstat->field_2E);
          }
          if ( effstat->field_30 )
              thing->health = 0;
        } else
        if (cube_is_lava(cube_id))
        {
            effmodel = effstat->field_31;
            if (effmodel > 0) {
                efftng = create_effect(prev_pos, effmodel, thing->owner);
                TRACE_THING(efftng);
            }
            sample_id = effstat->field_33;
            if (sample_id > 0) {
                thing_play_sample(efftng, sample_id, 100, 0, 3, 0, 2, effstat->field_35);
            }
            if ( effstat->field_37 )
                thing->health = 0;
        } else
        {
            effmodel = effstat->field_23;
            if (effmodel > 0) {
                efftng = create_effect(prev_pos, effmodel, thing->owner);
                TRACE_THING(efftng);
            }
            sample_id = effstat->field_25;
            if (sample_id > 0) {
                thing_play_sample(efftng, sample_id, 100, 0, 3, 0, 2, effstat->field_27);
            }
            if ( effstat->field_29 )
                thing->health = 0;
        }
    }
    remove_relevant_forces_from_thing_after_slide(thing, next_pos, blocked_flags);
}

TngUpdateRet move_effect_element(struct Thing *thing)
{
    struct Coord3d pos;
    TbBool move_allowed;
    SYNCDBG(18,"Starting");
    TRACE_THING(thing);
    //return _DK_move_effect_element(thing);
    move_allowed = get_thing_next_position(&pos, thing);
    if ( positions_equivalent(&thing->mappos, &pos) ) {
        return TUFRet_Unchanged;
    }
    if ((thing->movement_flags & TMvF_Unknown10) == 0)
    {
        if (!move_allowed)
        {
            move_effect_blocked(thing, &thing->mappos, &pos);
        } else
        if ( !thing_covers_same_blocks_in_two_positions(thing, &thing->mappos, &pos) )
        {
            if ( thing_in_wall_at(thing, &pos) )
            {
                move_effect_blocked(thing, &thing->mappos, &pos);
            }
        }
    }
    move_thing_in_map(thing, &pos);
    return TUFRet_Modified;
}

void change_effect_element_into_another(struct Thing *thing, long nmodel)
{
    SYNCDBG(18,"Starting");
    return _DK_change_effect_element_into_another(thing,nmodel);
}

TngUpdateRet update_effect_element(struct Thing *thing)
{
    struct EffectElementStats *eestats;
    long health;
    long abs_x,abs_y;
    long prop_factor,prop_val;
    long i;
    SYNCDBG(18,"Starting");
    TRACE_THING(thing);
    //return _DK_update_effect_element(thing);
    if (thing->model < sizeof(effect_element_stats)/sizeof(effect_element_stats[0])) {
        eestats = &effect_element_stats[thing->model];
    } else {
        ERRORLOG("Outranged model %d",(int)thing->model);
        eestats = &effect_element_stats[0];
    }
    // Check if effect health dropped to zero; delete it, or decrease health for the next check
    health = thing->health;
    if (health <= 0)
    {
        if (eestats->transform_model != 0)
        {
            change_effect_element_into_another(thing, eestats->transform_model);
        } else
        {
            delete_thing_structure(thing, 0);
        }
        return TUFRet_Deleted;
    }
    thing->health = health-1;
    // Set dynamic properties of the effect
    if (!eestats->field_12)
    {
        if (thing->field_60 >= (int)thing->mappos.z.val)
          thing->field_3E = 0;
    }
    if (eestats->field_15)
    {
        thing->movement_flags &= ~TMvF_IsOnWater;
        thing->movement_flags &= ~TMvF_IsOnLava;
        if (thing_touching_floor(thing))
        {
            i = get_top_cube_at(thing->mappos.x.stl.num, thing->mappos.y.stl.num);
            if (cube_is_water(i)) {
                thing->movement_flags |= TMvF_IsOnWater;
            } else
            if (cube_is_lava(i)) {
                thing->movement_flags |= TMvF_IsOnLava;
            }
        }
    }
    i = eestats->subeffect_delay;
    if (i > 0)
    {
      if (((thing->creation_turn - game.play_gameturn) % i) == 0) {
          create_effect_element(&thing->mappos, eestats->subeffect_model, thing->owner);
      }
    }
    switch (eestats->field_1)
    {
    case 1:
        move_effect_element(thing);
        break;
    case 2:
        i = thing->pos_2C.x.val;
        thing->pos_2C.x.val = 2*i/3;
        i = thing->pos_2C.y.val;
        thing->pos_2C.y.val = 2*i/3;
        i = thing->pos_2C.z.val;
        if (i > 32)
        {
          thing->pos_2C.z.val = 2*i/3;
        } else
        if (i > 16)
        {
          i = i-16;
          if (i < 16) i = 16;
          thing->pos_2C.z.val = i;
        } else
        if (i < -16)
        {
          thing->pos_2C.z.val = 2*i/3;
        } else
        {
            i = i+16;
            if (i > 16) i = 16;
            thing->pos_2C.z.val = i;
        }
        move_effect_element(thing);
        break;
    case 3:
        thing->pos_2C.z.val = 32;
        move_effect_element(thing);
        break;
    case 4:
        health = thing->health;
        if ((health >= 0) && (health < 16))
        {
            thing->pos_2C.z.val = bounce_table[health];
        } else
        {
            ERRORLOG("Illegal effect element bounce life: %ld", health);
        }
        move_effect_element(thing);
        break;
    case 5:
        break;
    default:
        ERRORLOG("Invalid effect element move type %d!",(int)eestats->field_1);
        move_effect_element(thing);
        break;
    }

    if (eestats->field_2 != 1)
      return TUFRet_Modified;
    abs_x = abs(thing->pos_2C.x.val);
    abs_y = abs(thing->pos_2C.y.val);
    prop_factor = LbDiagonalLength(abs_x, abs_y);
    i = ((LbArcTanAngle(thing->pos_2C.z.val, prop_factor) & 0x7FF) - 512) & 0x7FF;
    if (i > 1024)
      i -= 1024;
    prop_val = i / 128;
    thing->field_52 = LbArcTanAngle(thing->pos_2C.x.val, thing->pos_2C.y.val) & 0x7FF;
    thing->field_48 = prop_val;
    thing->field_3E = 0;
    thing->field_40 = (prop_val & 0xff) << 8;
    SYNCDBG(18,"Finished");
    return TUFRet_Modified;
}

struct Thing *create_effect_generator(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4)
{
  return _DK_create_effect_generator(pos, a1, a2, a3, a4);
}

long move_effect(struct Thing *thing)
{
  return _DK_move_effect(thing);
}

TbBool effect_can_affect_thing(struct Thing *efftng, struct Thing *thing)
{
    if (thing_is_invalid(efftng) || thing_is_invalid(thing))
    {
        WARNLOG("Invalid thing tries to interact with other things");
        return false;
    }
    if (thing->index == efftng->index)
    {
        SYNCDBG(18,"Effect tried to shoot itself; suicide not implemented");
        return false;
    }
    if (thing->index == efftng->parent_idx)
    {
        SYNCDBG(18,"Effect tried to shoot its maker; suicide not implemented");
        return false;
    }
    return explosion_can_affect_thing(thing, efftng->byte_16, efftng->owner);
}

void update_effect_light_intensity(struct Thing *thing)
{
  long i;
  if (thing->light_id != 0)
  {
      if (thing->health < 4)
      {
          i = light_get_light_intensity(thing->light_id);
          light_set_light_intensity(thing->light_id, (3*i)/4);
      }
  }
}

void effect_generate_effect_elements(const struct Thing *thing)
{
    const struct InitEffect *effnfo;
    struct PlayerInfo *player;
    struct Thing *elemtng;
    struct Coord3d pos;
    long i,k,n;
    long mag;
    unsigned long arg,argZ;
    effnfo = get_effect_info_for_thing(thing);
    SYNCDBG(18,"Preparing Effect, Generation Type %d",(int)effnfo->generation_type);
    switch (effnfo->generation_type)
    {
    case 1:
          for (i=0; i < effnfo->field_B; i++)
          {
              if (effnfo->kind_min <= 0)
                  continue;
              n = effnfo->kind_min + ACTION_RANDOM(effnfo->kind_max - effnfo->kind_min + 1);
              elemtng = create_effect_element(&thing->mappos, n, thing->owner);
              TRACE_THING(elemtng);
              if (thing_is_invalid(elemtng))
                break;
              arg = ACTION_RANDOM(0x800);
              argZ = ACTION_RANDOM(0x400);
              // Setting XY acceleration
              k = abs(effnfo->accel_xy_max - effnfo->accel_xy_min);
              if (k <= 1) k = 1;
              mag = effnfo->accel_xy_min + ACTION_RANDOM(k);
              elemtng->acceleration.x.val += distance_with_angle_to_coord_x(mag,arg);
              elemtng->acceleration.y.val += distance_with_angle_to_coord_y(mag,arg);
              // Setting Z acceleration
              k = abs(effnfo->accel_z_max - effnfo->accel_z_min);
              if (k <= 1) k = 1;
              mag = effnfo->accel_z_min + ACTION_RANDOM(k);
              elemtng->acceleration.z.val += distance_with_angle_to_coord_z(mag,argZ);
              elemtng->field_1 |= TF1_PushdByAccel;
          }
          break;
    case 2:
          k = 0;
          for (i=0; i < effnfo->field_B; i++)
          {
              n = effnfo->kind_min + ACTION_RANDOM(effnfo->kind_max - effnfo->kind_min + 1);
              mag = effnfo->start_health - thing->health;
              arg = (mag << 7) + k/effnfo->field_B;
              set_coords_to_cylindric_shift(&pos, &thing->mappos, mag, arg, 0);
              elemtng = create_effect_element(&pos, n, thing->owner);
              TRACE_THING(elemtng);
              SYNCDBG(18,"Created %s",thing_model_name(elemtng));
              k += 2048;
          }
          break;
    case 3:
          k = 0;
          for (i=0; i < effnfo->field_B; i++)
          {
              n = effnfo->kind_min + ACTION_RANDOM(effnfo->kind_max - effnfo->kind_min + 1);
              mag = thing->health;
              arg = (mag << 7) + k/effnfo->field_B;
              set_coords_to_cylindric_shift(&pos, &thing->mappos, 16*mag, arg, 0);
              elemtng = create_effect_element(&pos, n, thing->owner);
              TRACE_THING(elemtng);
              k += 2048;
          }
          break;
    case 4:
        if (thing->model != 48)
            break;
        i = effnfo->start_health / 2;
        if (thing->health == effnfo->start_health)
        {
            LbMemorySet(temp_pal, 63, PALETTE_SIZE);
        } else
        if (thing->health > i)
        {
          LbPaletteFade(temp_pal, i, Lb_PALETTE_FADE_OPEN);
        } else
        if (thing->health == i)
        {
          LbPaletteStopOpenFade();
          LbPaletteSet(temp_pal);
        } else
        if (thing->health > 0)
        {
            LbPaletteFade(engine_palette, 8, Lb_PALETTE_FADE_OPEN);
        } else
        {
            player = get_my_player();
            PaletteSetPlayerPalette(player, engine_palette);
            LbPaletteStopOpenFade();
        }
        break;
    default:
        ERRORLOG("Unknown Effect Generation Type %d",(int)effnfo->generation_type);
        break;
    }
}

TngUpdateRet process_effect_generator(struct Thing *thing)
{
    struct EffectGeneratorStats *egenstat;
    struct Thing *elemtng;
    struct Coord3d pos;
    long deviation_angle,deviation_mag;
    long i,k;
    SYNCDBG(18,"Starting");
    TRACE_THING(thing);
    //return _DK_process_effect_generator(thing);
    if (thing->health > 0)
        thing->health--;
    if (thing->health == 0)
    {
        delete_thing_structure(thing, 0);
        return TUFRet_Deleted;
    }
    if ( !any_player_close_enough_to_see(&thing->mappos) )
    {
        SYNCDBG(18,"No player sees %s at (%d,%d,%d)",thing_model_name(thing),(int)thing->mappos.x.stl.num,(int)thing->mappos.y.stl.num,(int)thing->mappos.z.stl.num);
        return TUFRet_Modified;
    }
    if (thing->long_15 > 0)
        thing->long_15--;
    if (thing->long_15 > 0)
    {
        return TUFRet_Modified;
    }
    egenstat = &effect_generator_stats[thing->model];
    for (i=0; i < egenstat->genation_amount; i++)
    {
        deviation_angle = ACTION_RANDOM(0x800);
        deviation_mag = ACTION_RANDOM(thing->word_13 + 1);
        set_coords_to_cylindric_shift(&pos, &thing->mappos, deviation_mag, deviation_angle, 0);
        SYNCDBG(18,"The %s creates effect %d/%d at (%d,%d,%d)",thing_model_name(thing),(int)pos.x.val,(int)pos.y.val,(int)pos.z.val);
        elemtng = create_effect_element(&pos, egenstat->field_C, thing->owner);
        TRACE_THING(elemtng);
        if (thing_is_invalid(elemtng))
            break;
        elemtng->sizexy = 20;
        elemtng->field_58 = 20;
        if (egenstat->field_10)
        {
            k = egenstat->field_11;
        } else
        if (egenstat->field_11 == -1)
        {
            elemtng->mappos.z.val = subtile_coord(8,0);
            k = get_next_gap_creature_can_fit_in_below_point(elemtng, &elemtng->mappos);
        } else
        {
            k = egenstat->field_11 + get_thing_height_at(elemtng, &elemtng->mappos);
        }
        elemtng->mappos.z.val = k;
        if ( thing_in_wall_at(elemtng, &elemtng->mappos) )
        {
            SYNCDBG(18,"The %s created effect %d/%d in wall, removing",thing_model_name(thing),(int)i,(int)egenstat->genation_amount);
            delete_thing_structure(elemtng, 0);
        } else
        {
            SYNCDBG(18,"The %s created effect %d/%d, index %d",thing_model_name(thing),(int)i,(int)egenstat->genation_amount,(int)elemtng->index);
            long acc_x,acc_y,acc_z;
            struct Thing *sectng;
            acc_x = egenstat->acc_x_min + ACTION_RANDOM(egenstat->acc_x_max - egenstat->acc_x_min + 1);
            acc_y = egenstat->acc_y_min + ACTION_RANDOM(egenstat->acc_y_max - egenstat->acc_y_min + 1);
            acc_z = egenstat->acc_z_min + ACTION_RANDOM(egenstat->acc_z_max - egenstat->acc_z_min + 1);
            elemtng->acceleration.x.val += acc_x;
            elemtng->acceleration.y.val += acc_y;
            elemtng->acceleration.z.val += acc_z;
            elemtng->field_1 |= TF1_PushdByAccel;
            if (egenstat->sound_sample_idx > 0)
            {
                sectng = create_effect(&elemtng->mappos, TngEff_Unknown49, thing->owner);
                TRACE_THING(sectng);
                if (!thing_is_invalid(sectng)) {
                    thing_play_sample(sectng, egenstat->sound_sample_idx + ACTION_RANDOM(egenstat->sound_sample_rng), 100, 0, 3, 0, 2, 256);
                }
            }
            if (egenstat->sound_sample_sec > 0) {
                thing_play_sample(elemtng, egenstat->sound_sample_sec, 100, 0, 3, 0, 2, 256);
            }
        }
    }
    thing->long_15 = egenstat->genation_delay_min + ACTION_RANDOM(egenstat->genation_delay_max - egenstat->genation_delay_min + 1);
    return TUFRet_Modified;
}

struct Thing *create_effect(const struct Coord3d *pos, ThingModel effmodel, PlayerNumber owner)
{
    struct Thing *thing;
    struct InitEffect *ieffect;
    //return _DK_create_effect(pos, effmodel, owner);
    ieffect = &effect_info[effmodel];
    if (!i_can_allocate_free_thing_structure(1)) {
        return INVALID_THING;
    }
    thing = allocate_free_thing_structure(1);
    if (thing->index == 0) {
        ERRORDBG(8,"Should be able to allocate effect %d for player %d, but failed.",(int)effmodel,(int)owner);
        return INVALID_THING;
    }
    thing->creation_turn = game.play_gameturn;
    thing->class_id = TCls_Effect;
    thing->model = effmodel;
    thing->mappos.x.val = pos->x.val;
    thing->mappos.y.val = pos->y.val;
    thing->mappos.z.val = pos->z.val;
    thing->next_on_mapblk = 0;
    thing->owner = owner;
    thing->parent_idx = thing->index;
    thing->field_20 = 0;
    thing->field_23 = 0;
    thing->field_24 = 0;
    thing->field_4F |= 0x01;
    thing->health = ieffect->start_health;
    if (ieffect->ilght.field_0 != 0)
    {
        struct InitLight ilght;
        memcpy(&ilght, &ieffect->ilght, sizeof(struct InitLight));
        ilght.is_dynamic = 1;
        ilght.mappos.x.val = thing->mappos.x.val;
        ilght.mappos.y.val = thing->mappos.y.val;
        ilght.mappos.z.val = thing->mappos.z.val;
        thing->light_id = light_create_light(&ilght);
        if (thing->light_id == 0) {
            // Note that there's an error here in original DK, and it makes unusable Thing entries if cannot allocate light.
            SYNCDBG(8,"Cannot allocate dynamic light to %s.",thing_model_name(thing));
        }
    }
    add_thing_to_its_class_list(thing);
    place_thing_in_mapwho(thing);
    if (ieffect->field_C != 0) {
        thing_play_sample(thing, ieffect->field_C, 100, 0, 3, 0, 3, 256);
    }
    return thing;
}

struct Thing *create_special_used_effect(const struct Coord3d *pos, long plyr_idx)
{
    struct Thing *efftng;
    efftng = create_effect(pos, TngEff_Unknown67, plyr_idx);
    TRACE_THING(efftng);
    return efftng;
}

TbBool destroy_effect_thing(struct Thing *thing)
{
    if (thing->model == 43)
    {
        place_slab_type_on_map(SlbT_LAVA, thing->mappos.x.stl.num, thing->mappos.y.stl.num, thing->owner, 0);
        do_slab_efficiency_alteration(subtile_slab_fast(thing->mappos.x.stl.num), subtile_slab_fast(thing->mappos.y.stl.num));
    }
    if (thing->snd_emitter_id != 0)
    {
        // In case of effect, don't stop any sound samples which are still playing
        S3DDestroySoundEmitter(thing->snd_emitter_id);
        thing->snd_emitter_id = 0;
    }
    delete_thing_structure(thing, 0);
    return true;
}

/**
 * Affects a thing with explosion effect.
 *
 * @param tngsrc The thing which caused the affect.
 * @param tngdst The thing being affected by the effect.
 * @param pos Position of the effect epicenter.
 * @param max_dist Max distance at which creatures are affected, in map coordinates.
 * @param max_damage Damage at epicenter of the explosion.
 * @param blow_strength The strength of hitwave blowing creatures out of affected area.
 * @param owner The owner of the explosion.
 * @return Gives true if the target thing was affected by the spell, false otherwise.
 * @note If the function returns true, the effect might have caused death of the target.
 */
TbBool explosion_affecting_thing(struct Thing *tngsrc, struct Thing *tngdst, const struct Coord3d *pos,
    MapCoord max_dist, HitPoints max_damage, long blow_strength, PlayerNumber owner)
{
    MapCoord distance;
    TbBool affected;
    affected = false;
    if ( line_of_sight_3d(pos, &tngdst->mappos) )
    {
        if ((tngdst->class_id == TCls_Creature) && (tngdst->owner == owner)) {
            max_dist /= 3;
        }
        distance = get_2d_distance(pos, &tngdst->mappos);
        if (distance < max_dist)
        {
            long move_dist,move_angle;
            move_angle = get_angle_xy_to(pos, &tngdst->mappos);
            if (tngdst->class_id == TCls_Creature)
            {
                HitPoints damage;
                damage = get_radially_decaying_value(max_damage,max_dist/4,3*max_dist/4,distance)+1;
                apply_damage_to_thing_and_display_health(tngdst, damage, owner);
                affected = true;
            }
            // If the thing isn't dying, move it
            if ((tngdst->class_id != TCls_Creature) || (tngdst->health >= 0))
            {
                move_dist = get_radially_decaying_value(blow_strength,max_dist/4,3*max_dist/4,distance);
                if (move_dist > 0)
                {
                    tngdst->acceleration.x.val += distance_with_angle_to_coord_x(move_dist, move_angle);
                    tngdst->acceleration.y.val += distance_with_angle_to_coord_y(move_dist, move_angle);
                    tngdst->field_1 |= TF1_PushdByAccel;
                    affected = true;
                }
            } else
            {
                kill_creature(tngdst, tngsrc, -1, CrDed_DiedInBattle);
                affected = true;
            }
        }
    }
    return affected;
}

/**
 * Computes and applies damage the Word Of Power spell makes to things at given map block.
 * @param efftng The effect thing which represents the spell.
 * @param tngsrc The thing being source of the spell.
 * @param mapblk Map block on which all targets are to be affected by the spell.
 * @param max_dist Range of the spell on map, used to compute damage decaying with distance; in map coordinates.
 * @param max_damage Damage at epicenter of the explosion.
 * @param blow_strength The strength of hitwave blowing creatures out of affected area.
 */
long word_of_power_affecting_map_block(struct Thing *efftng, struct Thing *tngsrc, struct Map *mapblk, MapCoord max_dist, HitPoints max_damage, long blow_strength)
{
    //TODO SPELLS This function should be replaced with explosion_affecting_map_block()
    struct Thing *thing;
    PlayerNumber owner;
    long num_affected;
    long i;
    unsigned long k;
    if (!thing_is_invalid(tngsrc))
        owner = tngsrc->owner;
    else
        owner = -1;
    num_affected = 0;
    k = 0;
    i = get_mapwho_thing_index(mapblk);
    while (i != 0)
    {
        thing = thing_get(i);
        TRACE_THING(thing);
        if (thing_is_invalid(thing))
        {
            ERRORLOG("Jump to invalid thing detected");
            break;
        }
        i = thing->next_on_mapblk;
        // Per thing processing block
        if (effect_can_affect_thing(efftng, thing)
          || ((thing->class_id == TCls_Door) && (thing->owner != tngsrc->owner)))
        {
            if (explosion_affecting_thing(tngsrc, thing, &efftng->mappos, max_dist, max_damage, blow_strength, owner))
                num_affected++;
        }
        // Per thing processing block ends
        k++;
        if (k > THINGS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping things list");
            break;
        }
    }
    return num_affected;
}

/**
 * Applies damage the Word Of Power spell makes to all things in the area surrounding given position.
 * @param efftng The thing which is WOP effect originator.
 * @param owntng The thing being affected by the spell.
 * @param pos Position where the WOP effect center is.
 * @param max_dist Range of the WOP spell effect, in map coordinates.
 */
void word_of_power_affecting_area(struct Thing *efftng, struct Thing *owntng, struct Coord3d *pos, MapCoord max_dist)
{
    struct Map *mapblk;
    long stl_xmin,stl_xmax;
    long stl_ymin,stl_ymax;
    long stl_x,stl_y;
    HitPoints max_damage;
    long blow_strength;
    //TODO CONFIG Damage and blow of WOP shouldn't be hardcoded
    max_damage = 150;
    blow_strength = 0;
    if (efftng->creation_turn != game.play_gameturn)
        return;
    {
        long stl_range;
        // Make sure the subtile is rounded up, unless the range is really close to lower value
        stl_range = coord_subtile(max_dist+240);
        // Position on subtile is not at its start, so add 1 to max values while ignoring the position
        stl_xmin = pos->x.stl.num - stl_range;
        stl_xmax = pos->x.stl.num + stl_range + 1;
        stl_ymin = pos->y.stl.num - stl_range;
        stl_ymax = pos->y.stl.num + stl_range + 1;
    }
    if (stl_xmin < 0) {
        stl_xmin = 0;
    } else
    if (stl_xmin > map_subtiles_x) {
        stl_xmin = map_subtiles_x;
    }
    if (stl_ymin < 0) {
      stl_ymin = 0;
    } else
    if (stl_ymin > map_subtiles_y) {
      stl_ymin = map_subtiles_y;
    }
    if (stl_xmax < 0) {
      stl_xmax = 0;
    } else
    if (stl_xmax > map_subtiles_x) {
      stl_xmax = map_subtiles_x;
    }
    if (stl_ymax < 0) {
      stl_ymax = 0;
    } else
    if (stl_ymax > map_subtiles_y) {
      stl_ymax = map_subtiles_y;
    }
    for (stl_y=stl_ymin; stl_y <= stl_ymax; stl_y++)
    {
        for (stl_x=stl_xmin; stl_x <= stl_xmax; stl_x++)
        {
            mapblk = get_map_block_at(stl_x, stl_y);
            word_of_power_affecting_map_block(efftng, owntng, mapblk, max_dist, max_damage, blow_strength);
        }
    }
}

/**
 * Determines if an explosion of given hit thing type and owner can affect given thing.
 * Explosions can affect a lot more things than shots. If only the thing isn't invalid,
 * it is by default affected by explosions.
 */
TbBool explosion_can_affect_thing(const struct Thing *thing, long hit_type, PlayerNumber shot_owner)
{
    if (thing_is_invalid(thing))
    {
        WARNLOG("Invalid thing tries to interact with explosion");
        return false;
    }
    switch (hit_type)
    {
    case THit_CrtrsNObjcts:
        return thing_is_shootable_by_any_player_including_objects(thing, shot_owner);
    case THit_CrtrsOnly:
        return thing_is_shootable_by_any_player_excluding_objects(thing, shot_owner);
    case THit_CrtrsNObjctsNotOwn:
        return thing_is_shootable_by_any_player_except_own_including_objects(thing, shot_owner);
    case THit_CrtrsOnlyNotOwn:
        return thing_is_shootable_by_any_player_except_own_excluding_objects(thing, shot_owner);
    case THit_CrtrsNotArmourNotOwn:
        return thing_is_shootable_by_any_player_except_own_excluding_objects_and_not_under_spell(thing, shot_owner, SplK_Armour);
    case THit_HeartOnly:
        if (thing_is_dungeon_heart(thing))
            return true;
        return false;
    case THit_HeartOnlyNotOwn:
        if (thing_is_dungeon_heart(thing) && (thing->owner != shot_owner))
          return true;
        return false;
    case THit_All:
        return true;
    case THit_None:
        return false;
    default:
        WARNLOG("Illegal hit thing type %d for shot owned by played %d",(int)hit_type,(int)shot_owner);
        return true;
    }
}

/**
 * Affects things on a map blocks with explosion effect, if only they should be affected with given hit type.
 *
 * @param tngsrc The thing which caused the affect.
 * @param mapblk Map blocks on which are creatures should be affected.
 * @param pos Position of the effect epicenter.
 * @param max_dist Max distance at which creatures are affected, in map coordinates.
 * @param max_damage Damage at epicenter of the explosion.
 * @param blow_strength The strength of hitwave blowing creatures out of affected area.
 * @param hit_type Defines which things are affected.
 */
long explosion_affecting_map_block(struct Thing *tngsrc, const struct Map *mapblk, const struct Coord3d *pos,
    MapCoord max_dist, HitPoints max_damage, long blow_strength, unsigned char hit_type)
{
    struct Thing *thing;
    PlayerNumber owner;
    long num_affected;
    unsigned long k;
    long i;
    if (!thing_is_invalid(tngsrc))
        owner = tngsrc->owner;
    else
        owner = -1;
    num_affected = 0;
    k = 0;
    i = get_mapwho_thing_index(mapblk);
    while (i != 0)
    {
        thing = thing_get(i);
        TRACE_THING(thing);
        if (thing_is_invalid(thing))
        {
            WARNLOG("Jump out of things array");
            break;
        }
        i = thing->next_on_mapblk;
        // Should never happen - only existing thing shall be in list
        if (!thing_exists(thing))
        {
            WARNLOG("Jump to non-existing thing");
            break;
        }
        // Per thing processing block
        if (explosion_can_affect_thing(thing, hit_type, owner))
        {
            if (explosion_affecting_thing(tngsrc, thing, pos, max_dist, max_damage, blow_strength, owner))
                num_affected++;
        }
        // Per thing processing block ends
        k++;
        if (k > THINGS_COUNT)
        {
            ERRORLOG("Infinite loop detected when sweeping things list");
            break;
        }
    }
    return num_affected;
}

/**
 * Affects things on an area with explosion effect, if only they should be affected with given hit type.
 *
 * @param tngsrc The thing which caused the effect.
 * @param pos Position of the effect epicenter.
 * @param range Range of the effect, in subtiles.
 * @param max_damage Damage at epicenter of the effect.
 * @param blow_strength The strength of hitwave blowing creatures out of affected area.
 * @param hit_type Defines which things are affected.
 * @return Gives amount of things which were affected by the explosion.
 */
long explosion_affecting_area(struct Thing *tngsrc, const struct Coord3d *pos,
    MapSubtlCoord range, HitPoints max_damage, long blow_strength, ThingHitType hit_type)
{
    const struct Map *mapblk;
    MapSubtlCoord start_x,start_y;
    MapSubtlCoord end_x,end_y;
    MapCoord max_dist;
    //_DK_explosion_affecting_area(tngsrc, pos, range, max_damage, hit_type); return;
    if ((hit_type < 1) || (hit_type >= THit_TypesCount))
    {
        ERRORLOG("The %s tries to affect area range %d with invalid hit type %d",thing_model_name(tngsrc),(int)range,(int)hit_type);
        hit_type = 1;
    }
    max_dist = (range << 8);
    if (pos->x.stl.num > range)
      start_x = pos->x.stl.num - range;
    else
      start_x = 0;
    if (pos->y.stl.num > range)
      start_y = pos->y.stl.num - range;
    else
      start_y = 0;
    end_x = range + pos->x.stl.num;
    if (end_x >= map_subtiles_x)
      end_x = map_subtiles_x;
    end_y = range + pos->y.stl.num;
    if (end_y > map_subtiles_y)
      end_y = map_subtiles_y;
#if (BFDEBUG_LEVEL > 0)
    if ((start_params.debug_flags & DFlg_ShotsDamage) != 0)
        create_price_effect(pos, my_player_number, max_damage);
#endif
    MapSubtlCoord stl_x,stl_y;
    long num_affected;
    num_affected = 0;
    for (stl_y = start_y; stl_y <= end_y; stl_y++)
    {
        for (stl_x = start_x; stl_x <= end_x; stl_x++)
        {
            mapblk = get_map_block_at(stl_x, stl_y);
            num_affected += explosion_affecting_map_block(tngsrc, mapblk, pos, max_dist, max_damage, blow_strength, hit_type);
        }
    }
    return num_affected;
}

void poison_cloud_affecting_area(struct Thing *owntng, struct Coord3d *pos, long a3, long a4, unsigned char area_affect_type)
{
    _DK_poison_cloud_affecting_area(owntng, pos, a3, a4, area_affect_type); return;
}

TngUpdateRet update_effect(struct Thing *efftng)
{
    const struct InitEffect *effnfo;
    struct Thing *subtng;
    SYNCDBG(18,"Starting for %s",thing_model_name(efftng));
    TRACE_THING(efftng);
    //return _DK_update_effect(thing);
    subtng = NULL;
    effnfo = get_effect_info_for_thing(efftng);
    if (efftng->parent_idx > 0) {
        subtng = thing_get(efftng->parent_idx);
        TRACE_THING(subtng);
    }
    if (efftng->health <= 0) {
        destroy_effect_thing(efftng);
        return TUFRet_Deleted;
    }
    update_effect_light_intensity(efftng);
    // Effect generators can be used to generate effect elements
    if ( (effnfo->field_11 == 0) || any_player_close_enough_to_see(&efftng->mappos) )
    {
        effect_generate_effect_elements(efftng);
    }
    // Let the effect affect area
    switch (effnfo->area_affect_type)
    {
    case 1:
    case 3:
        poison_cloud_affecting_area(subtng, &efftng->mappos, 1280, 60, effnfo->area_affect_type);
        break;
    case 4:
        word_of_power_affecting_area(efftng, subtng, &efftng->mappos, 1280);
        break;
    }
    efftng->health--;
    return move_effect(efftng);
}

struct Thing *create_price_effect(const struct Coord3d *pos, long plyr_idx, long price)
{
    struct Thing *elemtng;
    elemtng = create_effect_element(pos, TngEff_Unknown41, plyr_idx);
    TRACE_THING(elemtng);
    if (!thing_is_invalid(elemtng)) {
        elemtng->effect.number = price;
    }
    return elemtng;
}

/******************************************************************************/
#ifdef __cplusplus
}
#endif