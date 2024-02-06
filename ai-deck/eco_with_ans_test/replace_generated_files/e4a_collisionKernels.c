#include "e4a_collisionKernels.h"
L1_CL_MEM AT_L1_POINTER e4a_collision_L1_Memory;
L2_MEM AT_L2_POINTER e4a_collision_L2_Memory;
L2_MEM AT_L2_POINTER e4a_collision_L2_Memory_Dyn;
AT_HYPERRAM_POINTER e4a_collision_L3_Memory;
static AT_HYPERRAM_T HyperRam;
static AT_HYPERFLASH_FS_T HyperFlash;
void S1_Op_input_1_formatter(
		unsigned char * __restrict__ In,
		signed char * __restrict__ Out)

{
	/* Shared L1: 46400 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	KerNormBW_fps_T S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Iteration space related variables */
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[Tile0 Dim: 4]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 40000 [Tile0, 4:[200x58, 2:200x58, 200x26], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 4:[200x58, 2:200x58, 200x26], 1]
		Tile0: [0, 11600, 11600], Tile1: [11600, 11600, 11600], Tile2; [23200, 11600, 11600]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 40000 [Tile0, 4:[200x58, 2:200x58, 200x26], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 4:[200x58, 2:200x58, 200x26], 1]
		Tile0: [0, 11600, 11600], Tile1: [11600, 11600, 11600], Tile2; [23200, 11600, 11600]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->W = (unsigned short int) (200);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 11600, 0, DmaR_Evt1);
	_N_In=0;
	_C_Out=0; _SC_Out=11600;
	_SP_Out=0;
	/*============================= End Read Tiles Prolog ===============================*/
	for (T0Ind=0; T0Ind<4; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
		int T0Ind_Last = (T0Ind==3), T0Ind_NextLast = ((T0Ind+1)==3);
		/*================================= Prepare Tiles ===================================*/
		_SN_In = 0;
		if (!(T0Ind_Last)) {
			_N_In = _N_In + (11600); _SN_In = ((T0Ind_NextLast)?5200:11600); 
		}
		/*============================= End Prepare Tiles ===================================*/
		/*================================= Read Tiles ======================================*/
		AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
		if (_SN_In) {
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+11600*((T0Ind_Total+1)%2)),
					_SN_In, 0, DmaR_Evt1);
		}
		/*============================= End Read Tiles ======================================*/
		/*====================== Call Kernel LOC_LOOP =========================*/
		KerArg0->In = (unsigned char *__restrict__) (e4a_collision_L1_Memory+0+11600*((T0Ind_Total)%2));
		KerArg0->Out = (signed char *__restrict__) (e4a_collision_L1_Memory+23200+11600*((T0Ind_Total)%2));
		KerArg0->H = (unsigned short int) (T0Ind_Last?26:58);
		AT_FORK(gap_ncore(), (void *) CNN_NormBW_offset_fps, (void *) KerArg0);
		__CALL(CNN_NormBW_offset_fps, KerArg0);
		/*================================= Write Tiles =====================================*/
		if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
		AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+23200+11600*((T0Ind_Total)%2)),
				_SC_Out, 1, DmaW_Evt1);
		/*============================= End Write Tiles =====================================*/
		/*================================= Update Arg Pipeline =============================*/
		_SP_Out = _SC_Out;
		/*============================= End Update Arg Pipeline =============================*/
		/*================================= Prepare Tiles ===================================*/
		_SC_Out = 0;
		if (!(T0Ind_Last)) {
			_C_Out = _C_Out + (11600); _SC_Out = ((T0Ind_NextLast)?5200:11600); 
		}
		/*============================= End Prepare Tiles ===================================*/
	} /* End iteration on Tile0 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S4_Conv2d_4x1x5x5_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 46140 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	unsigned int _N_In;
	unsigned int _SN_In;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 4, Tiled: 1][Tile0 Dim: 8][D0 Dim: Init: 1, Tiled: 1]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 40000 [D1, [0 x 40000, 40000]][Tile0, 8:[100x14, 6:100x14, 100x2], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 40000, 40000]][Tile0, 8:[100x14, 6:100x14, 100x2], 1]
		Tile0: [0, 5600, 1400], Tile1: [1400, 5600, 1400], Tile2; [2800, 5600, 1400]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 4 [D1, [0 x 4, 4]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 4, 4]]
		Tile0: [0, 4, 4], Tile1: [0, 4, 4], Tile2; [0, 4, 4]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 4 [D1, [0 x 4, 4]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 4, 4]]
		Tile0: [0, 4, 4], Tile1: [0, 4, 4], Tile2; [0, 4, 4]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 100 [D1, [0 x 100, 100]][D0, [0 x 100, 100]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 100, 100]][D0, [0 x 100, 100]]
		Tile0: [0, 100, 100], Tile1: [0, 100, 100], Tile2; [0, 100, 100]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 40000 [D0, [0 x 40000, 40000]][Tile0, 8:[200x29, 6:200x31, 200x6], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 8:[200x28], 1][D0, [0 x 40000, 40000]]
		Tile0: [0, 5800, 5800], Tile1: [5200, 6200, 6200], Tile2; [10800, 6200, 6200]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 1 physical tiles
			Total Size: 160000 [D1, [0 x 160000, 160000]][Tile0, 8:[100x14, 6:100x14, 100x2], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 160000, 160000]][Tile0, 8:[100x14, 6:100x14, 100x2], 4]
		Tile0: [0, 22400, 5600], Tile1: [0, 22400, 5600], Tile2; [0, 22400, 5600]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 8:[13x1, 6:13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 8:[13x1, 6:13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+23724);
	KerArg0->W = (unsigned short int) (100);
	KerArg0->Feat = (unsigned short int) (4);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+12400);
	KerArg1->W = (unsigned short int) (200);
	KerArg1->UsedW = (unsigned short int) (200);
	KerArg1->InFeatures = (unsigned short int) (1);
	KerArg1->OutFeatures = (unsigned short int) (4);
	KerArg1->TotalInFeatures = (unsigned short int) (1);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+12424);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+23724);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+23724);
	KerArg2->Feat = (unsigned short int) (4);
	KerArg2->W = (unsigned short int) (100);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12416);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12420);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+46124);
	/*================================= Read Tiles Prolog ===============================*/
	_C_Out=0; _SC_Out=5600; _LC_Out=1400;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12400), 16, 0, DmaR_Evt1);
	AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12416), 4, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12420), 4, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12424), 100, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read Filter */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 5800, 0, DmaR_Evt5);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46124), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<8; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==7), T0Ind_NextLast = ((T0Ind+1)==7);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (5600-(400*(T0Ind==0))); _SN_In = (1*((T0Ind_NextLast)?1200:6200)); 
			} else if (!(1)) {
				_N_In = _N_In + (-38800); _SN_In = (1*(5800)); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+6200*((T0Ind_Total+1)%2)),
						_SN_In, 0, DmaR_Evt5);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?2:14);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+46124))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+6200*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?6:31)-2*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?6:31)-2*(T0Ind==0));
				KerArg1->Pad = (v4s) ((v4s){2,1,2*(T0Ind==0),1*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv5x5Stride2_SQ8, (void *) KerArg1);
				__CALL(KerParConv5x5Stride2_SQ8, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+12524+5600*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?2:14);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLU_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLU_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12524+5600*((T0Ind_Total)%2)),
					_SC_Out, 10000, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (1400); _LC_Out = ((T0Ind_NextLast)?200:1400); _SC_Out = (4*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S7_Conv2d_4x4x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 46584 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 4, Tiled: 1][Tile0 Dim: 8][D0 Dim: Init: 4, Tiled: 1]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 40000 [D0, [0 x 40000, 40000]][Tile0, 8:[100x15, 6:100x16, 100x3], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 8:[100x14], 1][D0, [0 x 40000, 40000]]
		Tile0: [0, 6000, 1500], Tile1: [1300, 6400, 1600], Tile2; [2700, 6400, 1600]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 4 [D1, [0 x 4, 4]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 4, 4]]
		Tile0: [0, 4, 4], Tile1: [0, 4, 4], Tile2; [0, 4, 4]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 4 [D1, [0 x 4, 4]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 4, 4]]
		Tile0: [0, 4, 4], Tile1: [0, 4, 4], Tile2; [0, 4, 4]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 144 [D1, [0 x 144, 144]][D0, [0 x 36, 36]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 144, 144]][D0, [0 x 36, 36]]
		Tile0: [0, 144, 144], Tile1: [0, 144, 144], Tile2; [0, 144, 144]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 40000 [D1, [0 x 40000, 40000]][Tile0, 8:[100x14, 6:100x14, 100x2], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 40000, 40000]][Tile0, 8:[100x14, 6:100x14, 100x2], 1]
		Tile0: [0, 5600, 1400], Tile1: [1400, 5600, 1400], Tile2; [2800, 5600, 1400]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 1 physical tiles
			Total Size: 160000 [D1, [0 x 160000, 160000]][Tile0, 8:[100x14, 6:100x14, 100x2], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 160000, 160000]][Tile0, 8:[100x14, 6:100x14, 100x2], 4]
		Tile0: [0, 22400, 5600], Tile1: [0, 22400, 5600], Tile2; [0, 22400, 5600]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 8 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 8:[13x1, 6:13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 8:[13x1, 6:13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+24168);
	KerArg0->W = (unsigned short int) (100);
	KerArg0->Feat = (unsigned short int) (4);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+12800);
	KerArg1->W = (unsigned short int) (100);
	KerArg1->UsedW = (unsigned short int) (100);
	KerArg1->InFeatures = (unsigned short int) (4);
	KerArg1->OutFeatures = (unsigned short int) (4);
	KerArg1->TotalInFeatures = (unsigned short int) (4);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+12824);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+24168);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+24168);
	KerArg2->Feat = (unsigned short int) (4);
	KerArg2->W = (unsigned short int) (100);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12816);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12820);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+46568);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 6000, 10000, 1500, 0, DmaR_Evt1);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12800), 16, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12816), 4, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12820), 4, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12824), 144, 0, DmaR_Evt5);
	AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read Filter */
	_C_Out=0; _SC_Out=5600; _LC_Out=1400;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46568), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<8; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==7), T0Ind_NextLast = ((T0Ind+1)==7);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (1400-(100*(T0Ind==0))); _LN_In = ((T0Ind_NextLast)?300:1600); _SN_In = (4*_LN_In); 
			} else if (!(1)) {
				_N_In = _N_In + (-9700); _LN_In = (1500); _SN_In = (4*_LN_In); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+6400*((T0Ind_Total+1)%2)),
						_SN_In, 10000, _LN_In, 0, DmaR_Evt1);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?2:14);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+46568))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+6400*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?3:16)-1*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?3:16)-1*(T0Ind==0));
				KerArg1->Pad = (v4s) ((v4s){1,1,1*(T0Ind==0),1*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv3x3Stride1_SQ8, (void *) KerArg1);
				__CALL(KerParConv3x3Stride1_SQ8, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+12968+5600*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?2:14);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLU_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLU_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12968+5600*((T0Ind_Total)%2)),
					_SC_Out, 10000, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (1400); _LC_Out = ((T0Ind_NextLast)?200:1400); _SC_Out = (4*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S10_Conv2d_8x4x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 45152 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 8, Tiled: 1][Tile0 Dim: 5][D0 Dim: Init: 4, Tiled: 1]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 5 logical tiles, 5 physical tiles
			Total Size: 20000 [D1, [0 x 20000, 20000]][Tile0, 5:[50x11, 3:50x11, 50x6], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 20000, 20000]][Tile0, 5:[50x11, 3:50x11, 50x6], 1]
		Tile0: [0, 4400, 550], Tile1: [550, 4400, 550], Tile2; [1100, 4400, 550]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 32 [D1, [0 x 32, 32]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 32, 32]]
		Tile0: [0, 32, 32], Tile1: [0, 32, 32], Tile2; [0, 32, 32]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 8 [D1, [0 x 8, 8]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 8, 8]]
		Tile0: [0, 8, 8], Tile1: [0, 8, 8], Tile2; [0, 8, 8]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 8 [D1, [0 x 8, 8]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 8, 8]]
		Tile0: [0, 8, 8], Tile1: [0, 8, 8], Tile2; [0, 8, 8]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 288 [D1, [0 x 36, 36]][D0, [0 x 288, 288]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 36, 36]][D0, [0 x 288, 288]]
		Tile0: [0, 288, 288], Tile1: [0, 288, 288], Tile2; [0, 288, 288]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 5 logical tiles, 5 physical tiles
			Total Size: 40000 [D0, [0 x 40000, 40000]][Tile0, 5:[100x22, 3:100x23, 100x13], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 5:[100x22], 1][D0, [0 x 40000, 40000]]
		Tile0: [0, 8800, 2200], Tile1: [2100, 9200, 2300], Tile2; [4300, 9200, 2300]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 5 logical tiles, 1 physical tiles
			Total Size: 80000 [D1, [0 x 80000, 80000]][Tile0, 5:[50x11, 3:50x11, 50x6], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 80000, 80000]][Tile0, 5:[50x11, 3:50x11, 50x6], 4]
		Tile0: [0, 17600, 2200], Tile1: [0, 17600, 2200], Tile2; [0, 17600, 2200]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 5 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 5:[13x1, 3:13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 5:[13x1, 3:13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+27536);
	KerArg0->W = (unsigned short int) (50);
	KerArg0->Feat = (unsigned short int) (8);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+18400);
	KerArg1->W = (unsigned short int) (100);
	KerArg1->UsedW = (unsigned short int) (100);
	KerArg1->InFeatures = (unsigned short int) (4);
	KerArg1->OutFeatures = (unsigned short int) (8);
	KerArg1->TotalInFeatures = (unsigned short int) (4);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+18448);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+27536);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+27536);
	KerArg2->Feat = (unsigned short int) (8);
	KerArg2->W = (unsigned short int) (50);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+18432);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+18440);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+45136);
	/*================================= Read Tiles Prolog ===============================*/
	_C_Out=0; _SC_Out=4400; _LC_Out=550;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+18400), 32, 0, DmaR_Evt1);
	AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+18432), 8, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+18440), 8, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+18448), 288, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read Filter */
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 8800, 10000, 2200, 0, DmaR_Evt5);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+45136), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<5; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==4), T0Ind_NextLast = ((T0Ind+1)==4);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (2200-(100*(T0Ind==0))); _LN_In = ((T0Ind_NextLast)?1300:2300); _SN_In = (4*_LN_In); 
			} else if (!(1)) {
				_N_In = _N_In + (-8700); _LN_In = (2200); _SN_In = (4*_LN_In); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+9200*((T0Ind_Total+1)%2)),
						_SN_In, 10000, _LN_In, 0, DmaR_Evt5);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?6:11);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+45136))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+9200*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?13:23)-1*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?13:23)-1*(T0Ind==0));
				KerArg1->Pad = (v4s) ((v4s){1,0,1*(T0Ind==0),0*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv3x3Stride2_SQ8, (void *) KerArg1);
				__CALL(KerParConv3x3Stride2_SQ8, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+18736+4400*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?6:11);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLU_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLU_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+18736+4400*((T0Ind_Total)%2)),
					_SC_Out, 2500, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (550); _LC_Out = ((T0Ind_NextLast)?300:550); _SC_Out = (8*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S13_Conv2d_8x8x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 43840 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 8, Tiled: 1][Tile0 Dim: 4][D0 Dim: Init: 8, Tiled: 1]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 20000 [D0, [0 x 20000, 20000]][Tile0, 4:[50x14, 2:50x15, 50x12], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 4:[50x13], 1][D0, [0 x 20000, 20000]]
		Tile0: [0, 5600, 700], Tile1: [600, 6000, 750], Tile2; [1250, 6000, 750]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 32 [D1, [0 x 32, 32]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 32, 32]]
		Tile0: [0, 32, 32], Tile1: [0, 32, 32], Tile2; [0, 32, 32]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 8 [D1, [0 x 8, 8]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 8, 8]]
		Tile0: [0, 8, 8], Tile1: [0, 8, 8], Tile2; [0, 8, 8]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 8 [D1, [0 x 8, 8]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 8, 8]]
		Tile0: [0, 8, 8], Tile1: [0, 8, 8], Tile2; [0, 8, 8]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 576 [D1, [0 x 576, 576]][D0, [0 x 72, 72]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 576, 576]][D0, [0 x 72, 72]]
		Tile0: [0, 576, 576], Tile1: [0, 576, 576], Tile2; [0, 576, 576]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 20000 [D1, [0 x 20000, 20000]][Tile0, 4:[50x13, 2:50x13, 50x11], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 20000, 20000]][Tile0, 4:[50x13, 2:50x13, 50x11], 1]
		Tile0: [0, 5200, 650], Tile1: [650, 5200, 650], Tile2; [1300, 5200, 650]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 4 logical tiles, 1 physical tiles
			Total Size: 80000 [D1, [0 x 80000, 80000]][Tile0, 4:[50x13, 2:50x13, 50x11], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 80000, 80000]][Tile0, 4:[50x13, 2:50x13, 50x11], 4]
		Tile0: [0, 20800, 2600], Tile1: [0, 20800, 2600], Tile2; [0, 20800, 2600]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 4 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 4:[13x1, 2:13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 4:[13x1, 2:13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+23024);
	KerArg0->W = (unsigned short int) (50);
	KerArg0->Feat = (unsigned short int) (8);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+12000);
	KerArg1->W = (unsigned short int) (50);
	KerArg1->UsedW = (unsigned short int) (50);
	KerArg1->InFeatures = (unsigned short int) (8);
	KerArg1->OutFeatures = (unsigned short int) (8);
	KerArg1->TotalInFeatures = (unsigned short int) (8);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+12048);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+23024);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+23024);
	KerArg2->Feat = (unsigned short int) (8);
	KerArg2->W = (unsigned short int) (50);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12032);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12040);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+43824);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 5600, 2500, 700, 0, DmaR_Evt1);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12000), 32, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12032), 8, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12040), 8, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12048), 576, 0, DmaR_Evt5);
	AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read Filter */
	_C_Out=0; _SC_Out=5200; _LC_Out=650;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+43824), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<4; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==3), T0Ind_NextLast = ((T0Ind+1)==3);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (650-(50*(T0Ind==0))); _LN_In = ((T0Ind_NextLast)?600:750); _SN_In = (8*_LN_In); 
			} else if (!(1)) {
				_N_In = _N_In + (-1900); _LN_In = (700); _SN_In = (8*_LN_In); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+6000*((T0Ind_Total+1)%2)),
						_SN_In, 2500, _LN_In, 0, DmaR_Evt1);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?11:13);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+43824))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+6000*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?12:15)-1*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?12:15)-1*(T0Ind==0));
				KerArg1->Pad = (v4s) ((v4s){1,1,1*(T0Ind==0),1*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv3x3Stride1_SQ8, (void *) KerArg1);
				__CALL(KerParConv3x3Stride1_SQ8, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+12624+5200*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?11:13);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLU_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLU_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12624+5200*((T0Ind_Total)%2)),
					_SC_Out, 2500, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (650); _LC_Out = ((T0Ind_NextLast)?550:650); _SC_Out = (8*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S16_Conv2d_16x8x5x5_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 46112 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Total=0, D0Ind_Last, D0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 16, Tiled: 1][Tile0 Dim: 2][D0 Dim: Init: 8, Tiled: 2]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 2 physical tiles
			Total Size: 10000 [D1, [0 x 10000, 10000]][Tile0, 2:[25x13, 25x12], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 10000, 10000]][Tile0, 2:[25x13, 25x12], 1]
		Tile0: [0, 5200, 325], Tile1: [325, 4800, 300], Tile2; [0, 5200, 325]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D1, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 3200 [D1, [0 x 3200, 3200]][D0, [1 x 1600, 1600]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 3200, 3200]][D0, [1 x 1600, 1600]]
		Tile0: [0, 3200, 3200], Tile1: [0, 3200, 3200], Tile2; [0, 3200, 3200]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 20000 [D0, [1 x 10000, 10000]][Tile0, 2:[50x27, 50x26], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 2:[50x27, 50x26], 1][D0, [1 x 10000, 10000]]
		Tile0: [0, 5400, 1350], Tile1: [10000, 5400, 1350], Tile2; [1200, 5200, 1300]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 40000 [D1, [0 x 40000, 40000]][Tile0, 2:[25x13, 25x12], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 40000, 40000]][Tile0, 2:[25x13, 25x12], 4]
		Tile0: [0, 20800, 1300], Tile1: [0, 20800, 1300], Tile2; [0, 20800, 1300]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 2:[13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 2:[13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+25296);
	KerArg0->W = (unsigned short int) (25);
	KerArg0->Feat = (unsigned short int) (16);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+11600);
	KerArg1->W = (unsigned short int) (50);
	KerArg1->UsedW = (unsigned short int) (50);
	KerArg1->InFeatures = (unsigned short int) (4);
	KerArg1->OutFeatures = (unsigned short int) (16);
	KerArg1->TotalInFeatures = (unsigned short int) (8);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+11696);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+25296);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+25296);
	KerArg2->Feat = (unsigned short int) (16);
	KerArg2->W = (unsigned short int) (25);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+11664);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+11680);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+46096);
	/*================================= Read Tiles Prolog ===============================*/
	_C_Out=0; _SC_Out=5200; _LC_Out=325;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+11600), 64, 0, DmaR_Evt1);
	AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+11664), 16, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+11680), 16, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+11696), 3200, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read Filter */
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 5400, 2500, 1350, 0, DmaR_Evt5);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46096), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<2; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==1), T0Ind_NextLast = ((T0Ind+1)==1);
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?12:13);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+46096))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			for (D0Ind=0; D0Ind<2; D0Ind++, D0Ind_Total++) { /* Iteration on D0 */
				int D0Ind_Last = (D0Ind==1), D0Ind_NextLast = ((D0Ind+1)==1);
				/*================================= Prepare Tiles ===================================*/
				_SN_In = 0;
				if (!(D0Ind_Last)) {
					_N_In = _N_In + (10000); _LN_In = ((T0Ind_Last)?1300:(1450-100*(T0Ind==0))); _SN_In = (4*_LN_In); 
				} else if (!(T0Ind_Last)) {
					_N_In = _N_In + (1300-(100*(T0Ind==0)))+(-10000); _LN_In = ((1)?1300:1450); _SN_In = (4*_LN_In); 
				} else if (!(1)) {
					_N_In = _N_In + (-1200)+(-10000); _LN_In = (1350); _SN_In = (4*_LN_In); 
				}
				/*============================= End Prepare Tiles ===================================*/
				/*================================= Read Tiles ======================================*/
				AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read In */
				if (_SN_In) {
					AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+5800*((D0Ind_Total+1)%2)),
							_SN_In, 2500, _LN_In, 0, DmaR_Evt5);
				}
				/*============================= End Read Tiles ======================================*/
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+5800*((D0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?26:29)-2*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?26:29)-2*(T0Ind==0));
				KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+11696+((D0Ind)*100));
				KerArg1->Pad = (v4s) ((v4s){2,1,2*(T0Ind==0),1*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv5x5Stride2_SQ8, (void *) KerArg1);
				__CALL(KerParConv5x5Stride2_SQ8, KerArg1);
				/*================================= Update Arg Pipeline =============================*/
				/*============================= End Update Arg Pipeline =============================*/
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+14896+5200*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?12:13);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLU_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLU_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+14896+5200*((T0Ind_Total)%2)),
					_SC_Out, 625, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (325); _LC_Out = ((1)?300:325); _SC_Out = (16*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S19_Conv2d_16x16x3x3_Relu(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 45616 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 16, Tiled: 1][Tile0 Dim: 2][D0 Dim: Init: 16, Tiled: 1]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 2 physical tiles
			Total Size: 10000 [D0, [0 x 10000, 10000]][Tile0, 2:[25x14, 25x13], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 2:[25x13], 1][D0, [0 x 10000, 10000]]
		Tile0: [0, 5600, 350], Tile1: [300, 5200, 325], Tile2; [0, 5600, 350]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D1, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 16 [D1, [0 x 16, 16]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 16, 16]]
		Tile0: [0, 16, 16], Tile1: [0, 16, 16], Tile2; [0, 16, 16]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 2304 [D1, [0 x 2304, 2304]][D0, [0 x 2304, 2304]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 2304, 2304]][D0, [0 x 2304, 2304]]
		Tile0: [0, 2304, 2304], Tile1: [0, 2304, 2304], Tile2; [0, 2304, 2304]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 2 physical tiles
			Total Size: 10000 [D1, [0 x 10000, 10000]][Tile0, 2:[25x13, 25x12], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 10000, 10000]][Tile0, 2:[25x13, 25x12], 1]
		Tile0: [0, 5200, 325], Tile1: [325, 4800, 300], Tile2; [0, 5200, 325]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 40000 [D1, [0 x 40000, 40000]][Tile0, 2:[25x13, 25x12], 4]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 40000, 40000]][Tile0, 2:[25x13, 25x12], 4]
		Tile0: [0, 20800, 1300], Tile1: [0, 20800, 1300], Tile2; [0, 20800, 1300]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 2:[13x1, 13x1], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 2:[13x1, 13x1], 1]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+24800);
	KerArg0->W = (unsigned short int) (25);
	KerArg0->Feat = (unsigned short int) (16);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+12000);
	KerArg1->W = (unsigned short int) (25);
	KerArg1->UsedW = (unsigned short int) (25);
	KerArg1->InFeatures = (unsigned short int) (16);
	KerArg1->OutFeatures = (unsigned short int) (16);
	KerArg1->TotalInFeatures = (unsigned short int) (16);
	KerArg1->Filter = (signed char * __restrict__) (e4a_collision_L1_Memory+12096);
	KerArg1->Out = (int * __restrict__) (e4a_collision_L1_Memory+24800);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+24800);
	KerArg2->Feat = (unsigned short int) (16);
	KerArg2->W = (unsigned short int) (25);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12064);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+12080);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+45600);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 5600, 625, 350, 0, DmaR_Evt1);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12000), 64, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12064), 16, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12080), 16, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+12096), 2304, 0, DmaR_Evt5);
	AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read Filter */
	_C_Out=0; _SC_Out=5200; _LC_Out=325;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+45600), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<2; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==1), T0Ind_NextLast = ((T0Ind+1)==1);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (325-(25*(T0Ind==0))); _LN_In = ((1)?325:375); _SN_In = (16*_LN_In); 
			} else if (!(1)) {
				_N_In = _N_In + (-300); _LN_In = (350); _SN_In = (16*_LN_In); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+6000*((T0Ind_Total+1)%2)),
						_SN_In, 625, _LN_In, 0, DmaR_Evt1);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?12:13);
			KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+45600))[8]);
			AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
			__CALL(KerParSetBiasB32_SQ8, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+6000*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (((T0Ind_Last)?13:15)-1*(T0Ind==0));
				KerArg1->UsedH = (unsigned short int) (((T0Ind_Last)?13:15)-1*(T0Ind==0));
				KerArg1->Pad = (v4s) ((v4s){1,1,1*(T0Ind==0),1*(T0Ind_Last)});
				AT_FORK(gap_ncore(), (void *) KerParConv3x3Stride1_SQ8, (void *) KerArg1);
				__CALL(KerParConv3x3Stride1_SQ8, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+14400+5200*((T0Ind_Total)%2));
			KerArg2->H = (unsigned short int) (T0Ind_Last?12:13);
			AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLUM_SQ8, (void *) KerArg2);
			__CALL(KerParReduct_CC_ReLUM_SQ8, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+14400+5200*((T0Ind_Total)%2)),
					_SC_Out, 625, _LC_Out, 1, DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (325); _LC_Out = ((1)?300:325); _SC_Out = (16*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S22_Op__classifier_classifier_0_Gemm_fusion(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 46616 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	AT_HYPERRAM_CL_EVENT _UchanHR1, *UchanHR1 = &_UchanHR1;
	KerSetBias_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerLinear_SQ8_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerConvLinReduct_SQ8_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D0Ind, D0Ind_Last, D0Ind_NextLast, D0Ind_NextNextLast;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast, T0Ind_NextNextLast;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _NN_Filter;
	unsigned int _SN_Filter, _SNN_Filter;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D0 Dim: Init: 64, Tiled: 1][Tile0 Dim: 29]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 29 logical tiles, 29 physical tiles
			Total Size: 10000 [Tile0, 29:[1x353, 27:1x353, 1x116], 1]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 29:[1x353, 27:1x353, 1x116], 1]
		Tile0: [0, 353, 353], Tile1: [353, 353, 353], Tile2; [706, 353, 353]
	Ker Arg: Filter, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 2
		KerArgItSpace: 29 logical tiles, 29 physical tiles
			Total Size: 640000 [D0, [0 x 640000, 640000]][Tile0, 29:[1x353, 27:1x353, 1x116], 1]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 640000, 640000]][Tile0, 29:[1x353, 27:1x353, 1x116], 1]
		Tile0: [0, 22592, 22592], Tile1: [22592, 22592, 22592], Tile2; [45184, 22592, 22592]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 256 [D0, [0 x 256, 256]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 256, 256]]
		Tile0: [0, 256, 256], Tile1: [0, 256, 256], Tile2; [0, 256, 256]
	Ker Arg: LinOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 256 [D0, [0 x 256, 256]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 256, 256]]
		Tile0: [0, 256, 256], Tile1: [0, 256, 256], Tile2; [0, 256, 256]
	Ker Arg: Out, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D0, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D0, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D0, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 29 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 29:[1x1, 27:1x1, 1x1], 13]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 29:[1x1, 27:1x1, 1x1], 13]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (int * __restrict__) (e4a_collision_L1_Memory+46152);
	KerArg0->W = (unsigned short int) (1);
	KerArg0->H = (unsigned short int) (1);
	KerArg0->Feat = (unsigned short int) (64);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+45896);
	KerArg1->Out = (void * __restrict__) (e4a_collision_L1_Memory+46152);
	KerArg1->OutDim = (unsigned short int) (64);
	KerArg1->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+46600);
	KerArg2->In = (int *__restrict__) (e4a_collision_L1_Memory+46152);
	KerArg2->Out = (void *__restrict__) (e4a_collision_L1_Memory+46408);
	KerArg2->Feat = (unsigned short int) (64);
	KerArg2->W = (unsigned short int) (1);
	KerArg2->H = (unsigned short int) (1);
	KerArg2->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+46472);
	KerArg2->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+46536);
	KerArg2->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+46600);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+0), 353, 0, DmaR_Evt1);
	_N_In=0;
	AT_HYPERRAM_CL_COPY(&HyperRam, ((AT_HYPERRAM_EXT_ADDR_TYPE) Filter+0), ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn+10064+0), 22592, 0, UchanHR1);
	AT_HYPERRAM_CL_WAIT(&HyperRam, UchanHR1); /* Wait previous uDMA read Filter */
	AT_HYPERRAM_CL_COPY(&HyperRam, ((AT_HYPERRAM_EXT_ADDR_TYPE) Filter+22592), ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn+10064+22592), 22592, 0, UchanHR1);
	AT_L2_COPY(0, ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn+10064+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+712+0), 22592, 0, DmaR_Evt2);
	_NN_Filter=22592; _SN_Filter=22592;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+45896), 256, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46472), 64, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46536), 64, 0, DmaR_Evt5);
	AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46600), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D0 */
		int D0Ind_Last = 1, D0Ind_NextLast = 1, D0Ind_NextNextLast = 1;
		/*====================== Call Kernel LOC_LOOP_PROLOG =========================*/
		KerArg0->NormBias = (unsigned char) (((char *)(e4a_collision_L1_Memory+46600))[8]);
		AT_FORK(gap_ncore(), (void *) KerParSetBiasB32_SQ8, (void *) KerArg0);
		__CALL(KerParSetBiasB32_SQ8, KerArg0);
		for (T0Ind=0; T0Ind<29; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==28), T0Ind_NextLast = ((T0Ind+1)==28), T0Ind_NextNextLast = ((T0Ind+2)==28);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (353); _SN_In = ((T0Ind_NextLast)?116:353); 
			} else if (!(1)) {
				_N_In = _N_In + (-9884); _SN_In = (353); 
			}
			_SNN_Filter = 0;
			if (!(T0Ind_Last)) {
				if (!(T0Ind_NextLast)) {
					_NN_Filter = _NN_Filter + (22592); _SNN_Filter = (((1)?(((T0Ind_NextNextLast)?7424:22592)):(((T0Ind_NextNextLast)?7424:22592)))); 
				}
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0+356*((T0Ind_Total+1)%2)),
						_SN_In, 0, DmaR_Evt1);
			}
			AT_HYPERRAM_CL_WAIT(&HyperRam, UchanHR1); /* Wait previous uDMA read Filter */
			if (_SNN_Filter) {
				AT_HYPERRAM_CL_COPY(&HyperRam, ((AT_HYPERRAM_EXT_ADDR_TYPE) Filter+_NN_Filter), ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn+10064+22592*((T0Ind_Total)%2)),
						_SNN_Filter, 0, UchanHR1);
			}
			AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Filter */
			if (_SN_Filter) {
				AT_L2_COPY(0, ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn+10064+22592*((T0Ind_Total+1)%2)), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+712+22592*((T0Ind_Total+1)%2)),
						_SN_Filter, 0, DmaR_Evt2);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_LOOP =========================*/
			KerArg1->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0+356*((T0Ind_Total)%2));
			KerArg1->Weights = (signed char * __restrict__) (e4a_collision_L1_Memory+712+22592*((T0Ind_Total)%2));
			KerArg1->InDim = (unsigned short int) (T0Ind_Last?116:353);
			KerArg1->TotalInDim = (unsigned short int) (T0Ind_Last?116:353);
			AT_FORK(gap_ncore(), (void *) KerParLinearLayer_SQ8, (void *) KerArg1);
			__CALL(KerParLinearLayer_SQ8, KerArg1);
			/*================================= Update Arg Pipeline =============================*/
			_SN_Filter = _SNN_Filter;
			/*============================= End Update Arg Pipeline =============================*/
		} /* End iteration on Tile0 */
		/*====================== Call Kernel LOC_LOOP_EPILOG =========================*/
		AT_FORK(gap_ncore(), (void *) KerParReduct_CC_ReLUM_SQ8, (void *) KerArg2);
		__CALL(KerParReduct_CC_ReLUM_SQ8, KerArg2);
	} /* End iteration on D0 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+46408), 64, 1, DmaW_Evt1);
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void S25_Op__classifier_classifier_3_Gemm_fusion(
		signed char * __restrict__ In,
		signed char * __restrict__ Filter,
		int * __restrict__ Bias,
		signed char * __restrict__ Out,
		unsigned char * __restrict__ Scale,
		signed char * __restrict__ ScaleN,
		signed char * __restrict__ Infos)

{
	/* Shared L1: 160 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaR_Evt3, *DmaR_Evt3 = &_DmaR_Evt3;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	AT_L2_EVENT _DmaR_Evt4, *DmaR_Evt4 = &_DmaR_Evt4;
	AT_L2_EVENT _DmaR_Evt5, *DmaR_Evt5 = &_DmaR_Evt5;
	AT_L2_EVENT _DmaR_Evt6, *DmaR_Evt6 = &_DmaR_Evt6;
	KerLinear_SQ8_T S_KerArg0, *KerArg0 = &S_KerArg0;
	expr_0_args_t S_KerArg1, *KerArg1 = &S_KerArg1;

	/* Iteration space related variables */
	int D0Ind, D0Ind_Last;
	int T0Ind, T0Ind_Last;
	/* User kernel arguments related variables */
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D0 Dim: Init: 1, Tiled: 1][Tile0 Dim: 1]
	Ker Arg: In, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [Tile0, 1:[1x1], 64]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 1:[1x1], 64]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D0, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 4 [D0, [0 x 4, 4]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 4, 4]]
		Tile0: [0, 4, 4], Tile1: [0, 4, 4], Tile2; [0, 4, 4]
	Ker Arg: Out, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 1 [D0, [0 x 1, 1]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 1, 1]]
		Tile0: [0, 1, 1], Tile1: [0, 1, 1], Tile2; [0, 1, 1]
	Ker Arg: Scale, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 1 [D0, [0 x 1, 1]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 1, 1]]
		Tile0: [0, 1, 1], Tile1: [0, 1, 1], Tile2; [0, 1, 1]
	Ker Arg: ScaleN, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 1 [D0, [0 x 1, 1]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 1, 1]]
		Tile0: [0, 1, 1], Tile1: [0, 1, 1], Tile2; [0, 1, 1]
	Ker Arg: Infos, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 13 [Tile0, 1:[1x1], 13]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 1:[1x1], 13]
		Tile0: [0, 13, 13], Tile1: [0, 13, 13], Tile2; [0, 13, 13]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->In = (signed char * __restrict__) (e4a_collision_L1_Memory+0);
	KerArg0->Weights = (signed char * __restrict__) (e4a_collision_L1_Memory+64);
	KerArg0->Bias = (void * __restrict__) (e4a_collision_L1_Memory+128);
	KerArg0->Out = (void * __restrict__) (e4a_collision_L1_Memory+132);
	KerArg0->InDim = (unsigned short int) (64);
	KerArg0->TotalInDim = (unsigned short int) (64);
	KerArg0->OutDim = (unsigned short int) (1);
	KerArg0->Scale = (unsigned char *__restrict__) (e4a_collision_L1_Memory+136);
	KerArg0->ScaleN = (unsigned char *__restrict__) (e4a_collision_L1_Memory+140);
	KerArg0->Infos = (signed char *__restrict__) (e4a_collision_L1_Memory+144);
	KerArg1->expr_0_in_0 = (signed char *__restrict__ ) (e4a_collision_L1_Memory+132);
	KerArg1->expr_0_out_0 = (signed char *__restrict__ ) (e4a_collision_L1_Memory+132);
	KerArg1->W = (unsigned short int) (1);
	KerArg1->H = (unsigned short int) (1);
	KerArg1->Feat = (unsigned short int) (1);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+0), 64, 0, DmaR_Evt1);
	AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read In */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+64), 64, 0, DmaR_Evt2);
	AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read Filter */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+128), 4, 0, DmaR_Evt3);
	AT_L2_WAIT(0, DmaR_Evt3); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Scale+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+136), 1, 0, DmaR_Evt4);
	AT_L2_WAIT(0, DmaR_Evt4); /* Wait previous DMA read Scale */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) ScaleN+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+140), 1, 0, DmaR_Evt5);
	AT_L2_WAIT(0, DmaR_Evt5); /* Wait previous DMA read ScaleN */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Infos+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+144), 13, 0, DmaR_Evt6);
	AT_L2_WAIT(0, DmaR_Evt6); /* Wait previous DMA read Infos */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D0 */
		int D0Ind_Last = 1;
		{ /* Single iteration on Tile0 */
			int T0Ind_Last = 1;
			/*====================== Call Kernel LOC_LOOP =========================*/
			AT_FORK(gap_ncore(), (void *) KerParLinearLayerFullFeatB32_SQ8, (void *) KerArg0);
			__CALL(KerParLinearLayerFullFeatB32_SQ8, KerArg0);
		} /* End iteration on Tile0 */
		/*====================== Call Kernel LOC_LOOP_EPILOG =========================*/
		AT_FORK(gap_ncore(), (void *) expr_0, (void *) KerArg1);
		__CALL(expr_0, KerArg1);
	} /* End iteration on D0 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+0), ((AT_L2_INT_ADDR_TYPE) e4a_collision_L1_Memory+132), 1, 1, DmaW_Evt1);
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
int e4a_collisionCNN_Construct()

{
	AT_HYPERRAM_CONF_T HyperRamConf;
	AT_HYPERFLASH_FS_CONF_T HyperFlashConf;

	int Error;
	AT_HYPERRAM_CONF_INIT(&HyperRamConf, AT_MEM_L3_HRAM, 0);
	AT_HYPERFLASH_FS_CONF_INIT(&HyperFlashConf, AT_MEM_L3_HFLASH, 0);
	AT_HYPERRAM_OPEN(&HyperRam, &HyperRamConf, &Error);
	if (Error) return 1;
	AT_HYPERFLASH_FS_OPEN(&HyperFlash, &HyperFlashConf, "e4a_collision_L3_Flash_Const.dat", &Error);
	if (Error) return 1;

	e4a_collision_L3_Memory = (AT_HYPERRAM_POINTER) AT_HYPERRAM_ALLOC(&HyperRam, 640000);
	if (e4a_collision_L3_Memory == 0) return 2;
	e4a_collision_L2_Memory = (AT_L2_POINTER) AT_L2_ALLOC(0, 7536);
	if (e4a_collision_L2_Memory == 0) return 3;
	e4a_collision_L2_Memory_Dyn = (AT_L2_POINTER) AT_L2_ALLOC(0, 80000);
	if (e4a_collision_L2_Memory_Dyn == 0) return 3;
	e4a_collision_L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, 46616);
	if (e4a_collision_L1_Memory == 0) return 4;
	AT_HYPERFLASH_FS_FC_EVENT _UchanHF1, *UchanHF1 = &_UchanHF1;
	AT_HYPERRAM_FC_EVENT _UchanHR2, *UchanHR2 = &_UchanHR2;
	/* Moving _classifier_classifier_0_gemm_weights, size 640000 from HyperFlash at 0 to (size 640000) HyperRam at 0..639999 */
	{
		int Size = 640000, Base = 0;
		while (Size) {
			int Chunk = Min(Size, 1024);
			AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 0 + Base), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn + 0), Chunk, 0, UchanHF1);
			AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
			AT_HYPERRAM_FC_COPY(&HyperRam, ((AT_HYPERRAM_EXT_ADDR_TYPE) e4a_collision_L3_Memory + 0 + Base), ((AT_HYPERRAM_INT_ADDR_TYPE) e4a_collision_L2_Memory_Dyn + 0), Chunk, 1, UchanHR2);
			AT_HYPERRAM_FC_WAIT(&HyperRam, UchanHR2);
			Base += Chunk;
			Size -= Chunk;
		}
	}
	/* Moving _features_features_0_conv_weights, size 100 from HyperFlash at 646768 to (size 100) L2 at 6768..6867 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646768), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6768), 100, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_0_bias, size 16 from HyperFlash at 647252 to (size 16) L2 at 7252..7267 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647252), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7252), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S4_Mul_scale, size 4 from HyperFlash at 647508 to (size 4) L2 at 7508..7511 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647508), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7508), 4, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S4_Mul_shift, size 4 from HyperFlash at 647512 to (size 4) L2 at 7512..7515 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647512), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7512), 4, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S4_Infos, size 13 from HyperFlash at 647268 to (size 13) L2 at 7268..7280 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647268), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7268), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _features_features_2_conv_weights, size 144 from HyperFlash at 646624 to (size 144) L2 at 6624..6767 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646624), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6624), 144, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_2_bias, size 16 from HyperFlash at 647284 to (size 16) L2 at 7284..7299 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647284), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7284), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S7_Mul_scale, size 4 from HyperFlash at 647516 to (size 4) L2 at 7516..7519 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647516), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7516), 4, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S7_Mul_shift, size 4 from HyperFlash at 647520 to (size 4) L2 at 7520..7523 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647520), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7520), 4, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S7_Infos, size 13 from HyperFlash at 647300 to (size 13) L2 at 7300..7312 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647300), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7300), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _features_features_4_conv_weights, size 288 from HyperFlash at 646080 to (size 288) L2 at 6080..6367 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646080), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6080), 288, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_4_bias, size 32 from HyperFlash at 647188 to (size 32) L2 at 7188..7219 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647188), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7188), 32, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S10_Mul_scale, size 8 from HyperFlash at 647476 to (size 8) L2 at 7476..7483 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647476), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7476), 8, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S10_Mul_shift, size 8 from HyperFlash at 647484 to (size 8) L2 at 7484..7491 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647484), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7484), 8, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S10_Infos, size 13 from HyperFlash at 647316 to (size 13) L2 at 7316..7328 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647316), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7316), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _features_features_6_conv_weights, size 576 from HyperFlash at 645504 to (size 576) L2 at 5504..6079 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 645504), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 5504), 576, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_6_bias, size 32 from HyperFlash at 647220 to (size 32) L2 at 7220..7251 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647220), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7220), 32, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S13_Mul_scale, size 8 from HyperFlash at 647492 to (size 8) L2 at 7492..7499 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647492), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7492), 8, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S13_Mul_shift, size 8 from HyperFlash at 647500 to (size 8) L2 at 7500..7507 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647500), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7500), 8, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S13_Infos, size 13 from HyperFlash at 647332 to (size 13) L2 at 7332..7344 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647332), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7332), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _features_features_8_conv_weights, size 3200 from HyperFlash at 640000 to (size 3200) L2 at 0..3199 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 640000), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 0), 3200, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_8_bias, size 64 from HyperFlash at 646868 to (size 64) L2 at 6868..6931 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646868), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6868), 64, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S16_Mul_scale, size 16 from HyperFlash at 647348 to (size 16) L2 at 7348..7363 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647348), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7348), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S16_Mul_shift, size 16 from HyperFlash at 647364 to (size 16) L2 at 7364..7379 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647364), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7364), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S16_Infos, size 13 from HyperFlash at 647380 to (size 13) L2 at 7380..7392 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647380), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7380), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _features_features_10_conv_weights, size 2304 from HyperFlash at 643200 to (size 2304) L2 at 3200..5503 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 643200), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 3200), 2304, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving Constant_features_10_bias, size 64 from HyperFlash at 646932 to (size 64) L2 at 6932..6995 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646932), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6932), 64, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S19_Mul_scale, size 16 from HyperFlash at 647396 to (size 16) L2 at 7396..7411 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647396), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7396), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S19_Mul_shift, size 16 from HyperFlash at 647412 to (size 16) L2 at 7412..7427 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647412), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7412), 16, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S19_Infos, size 13 from HyperFlash at 647428 to (size 13) L2 at 7428..7440 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647428), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7428), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _classifier_classifier_0_gemm_biases, size 256 from HyperFlash at 646368 to (size 256) L2 at 6368..6623 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646368), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6368), 256, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S22_Mul_scale, size 64 from HyperFlash at 646996 to (size 64) L2 at 6996..7059 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 646996), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 6996), 64, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S22_Mul_shift, size 64 from HyperFlash at 647060 to (size 64) L2 at 7060..7123 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647060), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7060), 64, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S22_Infos, size 13 from HyperFlash at 647444 to (size 13) L2 at 7444..7456 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647444), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7444), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _classifier_classifier_3_gemm_weights, size 64 from HyperFlash at 647124 to (size 64) L2 at 7124..7187 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647124), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7124), 64, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving _classifier_classifier_3_gemm_biases, size 4 from HyperFlash at 647524 to (size 4) L2 at 7524..7527 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647524), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7524), 4, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S25_Mul_scale, size 1 from HyperFlash at 647528 to (size 1) L2 at 7528..7528 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647528), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7528), 1, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S25_Mul_shift, size 1 from HyperFlash at 647532 to (size 1) L2 at 7532..7532 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647532), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7532), 1, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	/* Moving S25_Infos, size 13 from HyperFlash at 647460 to (size 13) L2 at 7460..7472 */
	AT_HYPERFLASH_FS_FC_COPY(&HyperFlash, ((AT_HYPERFLASH_FS_EXT_ADDR_TYPE) e4a_collision_L3_Flash + 647460), ((AT_HYPERFLASH_FS_INT_ADDR_TYPE) e4a_collision_L2_Memory + 7460), 13, 0, UchanHF1);
	AT_HYPERFLASH_FS_FC_WAIT(&HyperFlash, UchanHF1);
	return 0;
}
int e4a_collisionCNN_Destruct()

{
	AT_HYPERRAM_FREE(&HyperRam, e4a_collision_L3_Memory, 640000);
	AT_L2_FREE(0, e4a_collision_L2_Memory_Dyn, 80000);
	AT_L2_FREE(0, e4a_collision_L2_Memory, 7536);
	AT_L1_FREE(0, e4a_collision_L1_Memory, 46616);
	AT_HYPERRAM_CLOSE(&HyperRam);
	AT_HYPERFLASH_FS_CLOSE(&HyperFlash);
	return 0;
}
int e4a_collisionCNN_Memory(int Which)

{
	switch (Which) {
		case 0: return 46616;
		case 1: return 80000;
		case 2: return 7536;
	}
	return 0;
}
unsigned int AT_GraphOperInfosNames[10] = {
	40000,
	1040000,
	1480000,
	740000,
	1460000,
	2010000,
	1450000,
	640064,
	65,
	0,
};
int e4a_collisionCNN(
		unsigned char * __restrict__ Input_1,
		signed char * __restrict__ Output_1,
		uint8_t cp)

{	

	S1_Op_input_1_formatter(
		((unsigned char * __restrict__) Input_1), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)) /* Out */
	);
	if(cp == 0)
		return cp;
	S4_Conv2d_4x1x5x5_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+6768)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+7252)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+40000)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7508)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7512)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7268)) /* Infos */
	);

	if(cp == 1)
		return cp;

	S7_Conv2d_4x4x3x3_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+40000)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+6624)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+7284)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7516)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7520)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7300)) /* Infos */
	);

	if(cp == 2)
		return cp;

	S10_Conv2d_8x4x3x3_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+6080)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+7188)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+40000)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7476)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7484)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7316)) /* Infos */
	);

	if(cp == 3)
		return cp;

	S13_Conv2d_8x8x3x3_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+40000)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+5504)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+7220)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7492)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7500)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7332)) /* Infos */
	);

	if(cp == 4)
		return cp;

	S16_Conv2d_16x8x5x5_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+0)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+6868)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+20000)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7348)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7364)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7380)) /* Infos */
	);

	if(cp == 5)
		return cp;

	S19_Conv2d_16x16x3x3_Relu(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+20000)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+3200)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+6932)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+64)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7396)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7412)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7428)) /* Infos */
	);

	if(cp == 6)
		return cp;

	S22_Op__classifier_classifier_0_Gemm_fusion(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+64)), /* In */
		((signed char * __restrict__) (e4a_collision_L3_Memory+0)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+6368)), /* Bias */
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+6996)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7060)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7444)) /* Infos */
	);

	if(cp == 7)
		return cp;

	S25_Op__classifier_classifier_3_Gemm_fusion(
		((signed char * __restrict__) (e4a_collision_L2_Memory_Dyn+0)), /* In */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7124)), /* Filter */
		((signed int * __restrict__) (e4a_collision_L2_Memory+7524)), /* Bias */
		((signed char * __restrict__) Output_1), /* Out */
		((unsigned char * __restrict__) (e4a_collision_L2_Memory+7528)), /* Scale */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7532)), /* ScaleN */
		((signed char * __restrict__) (e4a_collision_L2_Memory+7460)) /* Infos */
	);
	return cp;
}
