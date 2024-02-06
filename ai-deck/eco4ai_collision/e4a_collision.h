
#ifndef __E4A_COLLISION_H__
#define __E4A_COLLISION_H__


#include "Gap.h"

#define __PREFIX(x) e4a_collision##x

extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash);

typedef struct
{
  uint32_t w;
  uint32_t h;
  uint32_t c;
  uint8_t cp;
} __attribute__((packed)) InterHeader;

typedef struct
{
    int offset;
    char *name;
    unsigned c;
    unsigned h;
    unsigned w;
} tensor;

typedef enum {NO, S4, S7, S10, S13, S16, S19, S22, ALL} CuttingPoint;

#endif