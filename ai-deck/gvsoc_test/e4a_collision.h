#ifndef __E4A_COLLISION_H__
#define __E4A_COLLISION_H__

#include "e4a_collisionKernels.h"

extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE e4a_collision_L3_Flash;

#include "Gap.h"
#include "gaplib/ImgIO.h"
// #include "measurments_utils.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif

// CP => Cutting Point
#define NB_CP 7

typedef struct
{
    unsigned c;
    unsigned h;
    unsigned w;
    int offset;
} tensor;




#endif
