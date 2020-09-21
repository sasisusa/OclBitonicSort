
typedef SORTTYPE SORTDATA;

#define CmpXchg(Loc, Perm, a, b)		\
{										\
	unsigned int uiTmp;					\
	SORTDATA TmpA = *(Loc + a);			\
	SORTDATA TmpB = *(Loc + b); 		\
										\
	if(TmpA > TmpB){					\
		*(Loc + a) = TmpB;				\
		*(Loc + b) = TmpA;				\
		uiTmp = *(Perm + a);			\
		*(Perm + a) = *(Perm + b);		\
		*(Perm + b) = uiTmp;			\
	}									\
}


__kernel __attribute__((reqd_work_group_size(L_WZ_I, 1, 1))) void BitonicSortStage(__global SORTDATA *sortData, __global unsigned int *puiPerm)
{
	unsigned int L_ID, Pos, LenArrow, uiStage, uiLevel, ui;
	__local SORTDATA sortLocTmp[M_FOR*L_WZ_I];
	__local unsigned int PermTmp[M_FOR*L_WZ_I];

	L_ID = get_local_id(0);
	Pos = M_FOR*get_group_id(0)*L_WZ_I + L_ID;
	#ifdef UNROLL_BSS_MEMCPY
	#pragma unroll
	#endif
	for(ui=0; ui<M_FOR; ++ui){
		sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
	}
	#ifdef UNROLL_BSS_MEMCPY
	#pragma unroll
	#endif
	for(ui=0; ui<M_FOR; ++ui){
		PermTmp[L_ID + ui*L_WZ_I] = puiPerm[Pos+ui*L_WZ_I];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	#ifdef UNROLL_BSS_STAGE
	#pragma unroll
	#endif
	for(uiStage=1; uiStage<M_FOR*L_WZ_I; uiStage*=2){
		for(ui=0; ui<(M_FOR/2); ++ui){
			Pos = ((L_ID + ui*L_WZ_I)&(~(uiStage-1))) + (L_ID + ui*L_WZ_I);
			LenArrow = 2*(uiStage - ((L_ID + ui*L_WZ_I)&(uiStage-1))) - 1;
			CmpXchg(sortLocTmp, PermTmp, Pos, Pos+LenArrow);
		}	
		barrier(CLK_LOCAL_MEM_FENCE);
		
		#ifdef UNROLL_BSS_LEVEL
		#pragma unroll
		#endif
		for(uiLevel=uiStage>>1; uiLevel>0; uiLevel>>=1){
			for(ui=0; ui<(M_FOR/2); ++ui){
				Pos = ((L_ID + ui*L_WZ_I)&(~(uiLevel-1))) + (L_ID + ui*L_WZ_I);
				CmpXchg(sortLocTmp, PermTmp, Pos, Pos+uiLevel);
			}		
			barrier(CLK_LOCAL_MEM_FENCE);
		}		
	}
	
	Pos = M_FOR*get_group_id(0)*L_WZ_I + L_ID;
	#ifdef UNROLL_BSS_MEMCPY
	#pragma unroll
	#endif	
	for(ui=0; ui<M_FOR; ++ui){
		sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
	}
	#ifdef UNROLL_BSS_MEMCPY
	#pragma unroll
	#endif	
	for(ui=0; ui<M_FOR; ++ui){
		puiPerm[Pos + ui*L_WZ_I] = PermTmp[L_ID + ui*L_WZ_I];
	}	
}


__kernel __attribute__((reqd_work_group_size(L_WZ_I, 1, 1))) void BitonicSortLevel(__global SORTDATA *sortData, __global unsigned int *puiPerm)
{
	unsigned int L_ID, Pos, uiLevel, ui;
	__local SORTDATA sortLocTmp[M_FOR*L_WZ_I];
	__local unsigned int PermTmp[M_FOR*L_WZ_I];
	
	L_ID = get_local_id(0);
	Pos = M_FOR*get_group_id(0)*L_WZ_I + L_ID;

	#ifdef UNROLL_BSL_MEMCPY
	#pragma unroll
	#endif	
	for(ui=0; ui<M_FOR; ++ui){
		sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
	}	
	#ifdef UNROLL_BSL_MEMCPY
	#pragma unroll
	#endif	
	for(ui=0; ui<M_FOR; ++ui){
		PermTmp[L_ID + ui*L_WZ_I] = puiPerm[Pos+ui*L_WZ_I];
	}	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	#ifdef UNROLL_BSL_LEVEL
	#pragma unroll
	#endif
	for(uiLevel=(M_FOR/2)*L_WZ_I; uiLevel>0; uiLevel>>=1){
		for(ui=0; ui<(M_FOR/2); ++ui){
			Pos = ((L_ID + ui*L_WZ_I)&(~(uiLevel-1))) + (L_ID + ui*L_WZ_I);
			CmpXchg(sortLocTmp, PermTmp, Pos, Pos+uiLevel);
		}				
		barrier(CLK_LOCAL_MEM_FENCE);
	}		

	Pos = M_FOR*get_group_id(0)*L_WZ_I + L_ID;
	#ifdef UNROLL_BSL_MEMCPY
	#pragma unroll
	#endif
	for(ui=0; ui<M_FOR; ++ui){
		sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
	}	
	#ifdef UNROLL_BSL_MEMCPY
	#pragma unroll
	#endif
	for(ui=0; ui<M_FOR; ++ui){
		puiPerm[Pos + ui*L_WZ_I] = PermTmp[L_ID + ui*L_WZ_I];
	}	
}

__kernel __attribute__((reqd_work_group_size(L_WZ, 1, 1))) void BitonicSort(__global SORTDATA *sortData, __global unsigned int *puiPerm, unsigned int uiLevel)
{
	unsigned int G_ID, Pos;
	
	G_ID = get_global_id(0);
	Pos = (G_ID&(~(uiLevel-1))) + G_ID;
	CmpXchg(sortData, puiPerm, Pos, Pos+uiLevel);
}

__kernel __attribute__((reqd_work_group_size(L_WZ, 1, 1))) void BitonicMerge(__global SORTDATA *sortData, __global unsigned int *puiPerm, unsigned int uiLevel)
{
	unsigned int G_ID, Pos, LenArrow;
	
	G_ID = get_global_id(0);
	Pos = (G_ID&(~(uiLevel-1))) + G_ID;
	LenArrow = 2*(uiLevel - (G_ID&(uiLevel-1))) - 1;
	CmpXchg(sortData, puiPerm, Pos, Pos+LenArrow);
}
