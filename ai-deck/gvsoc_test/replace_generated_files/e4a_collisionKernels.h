#ifndef __E4A_COLLISIONKERNEL_H__
#define __E4A_COLLISIONKERNEL_H__

#include "AutoTilerLibTypes.h"
#include "Gap.h"
#include "e4a_collision.h"
#include "CNN_BasicKernels_SQ8.h"
#include "Expression_Kernels.h"

#include "eco_type.h"

#define _e4a_collision_L1_Memory_SIZE 43496
#define _e4a_collision_L2_Memory_SIZE 7536
#define _e4a_collision_L2_Memory_Dyn_SIZE 80000
extern char *e4a_collision_L1_Memory; /* Size given for generation: 43616 bytes, used: 43496 bytes */
extern char *e4a_collision_L2_Memory; /* Size used for generation (static): 7536 bytes */
extern char *e4a_collision_L2_Memory_Dyn; /* Size used for generation (dynamic): 80000 bytes */
extern void S1_Op_input_1_formatter(
		unsigned char * __restrict__ In,
		signed char * __restrict__ Out);
extern void S4_Conv2d_4x1x5x5_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S7_Conv2d_4x4x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S10_Conv2d_8x4x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S13_Conv2d_8x8x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S16_Conv2d_16x8x5x5_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S19_Conv2d_16x16x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S22_Op__classifier_classifier_0_Gemm_fusion(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern void S25_Op__classifier_classifier_3_Gemm_fusion(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos);
extern int e4a_collisionCNN_Construct();
extern int e4a_collisionCNN_Destruct();
extern int e4a_collisionCNN_Memory(int Which);
extern unsigned char * __restrict__ Input_1;
extern int e4a_collisionCNN(
		signed char * __restrict__ Output_1, CP cp);
extern unsigned int AT_GraphPerf[10];
extern unsigned int AT_GraphPerf_CNN_Total;
extern char * AT_GraphNodeNames[10];
extern unsigned int AT_GraphOperInfosNames[10];
#endif
