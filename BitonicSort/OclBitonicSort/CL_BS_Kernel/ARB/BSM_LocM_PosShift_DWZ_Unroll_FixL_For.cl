
typedef SORTTYPE SORTDATA;

#define CmpXchg(A, B)		\
{							\
	SORTDATA TmpA = *(A);	\
	SORTDATA TmpB = *(B); 	\
							\
	if(TmpA > TmpB){		\
		*(A) = TmpB;		\
		*(B) = TmpA;		\
	}						\
}


__kernel __attribute__((reqd_work_group_size(L_WZ_I, 1, 1))) void BitonicSortStage(__global SORTDATA *sortData)
{
	unsigned int L_ID, Pos, LenArrow, ui, Group;
	int iStage, iLevel;
	__local SORTDATA sortLocTmp[M_FOR*L_WZ_I];
	
	L_ID = get_local_id(0);
	Group = M_FOR*L_WZ_I*get_group_id(0);
	Pos = Group + L_ID;
	
	if(Group + M_FOR*L_WZ_I < N_KEYS){
		#ifdef UNROLL_BSS_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
		}	
		barrier(CLK_LOCAL_MEM_FENCE);
		
		#ifdef UNROLL_BSS_STAGE
		#pragma unroll
		#endif
		for(iStage=0; iStage<(ILOG2); ++iStage){
			for(ui=0; ui<(M_FOR/2); ++ui){
				Pos = (((L_ID + ui*L_WZ_I)>>iStage)<<iStage) + (L_ID + ui*L_WZ_I);
				LenArrow = 1 << iStage;
				LenArrow = 2*(LenArrow - ((L_ID + ui*L_WZ_I)&(LenArrow-1))) - 1;
				CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			
			#ifdef UNROLL_BSS_LEVEL
			#pragma unroll
			#endif
			for(iLevel=iStage-1; iLevel>=0; --iLevel){
				LenArrow = 1 << iLevel;
				for(ui=0; ui<(M_FOR/2); ++ui){
					Pos = (((L_ID + ui*L_WZ_I)>>iLevel)<<iLevel) + (L_ID + ui*L_WZ_I);
					CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
				}				
				barrier(CLK_LOCAL_MEM_FENCE);
			}		
		}
		
		Pos = Group + L_ID;
		#ifdef UNROLL_BSS_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
		}
	}	
	else{
		#ifdef UNROLL_BSS_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			if(Pos+ui*L_WZ_I < N_KEYS){
				sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
			}
		}	
		barrier(CLK_LOCAL_MEM_FENCE);
		
		#ifdef UNROLL_BSS_STAGE
		#pragma unroll
		#endif
		for(iStage=0; iStage<(ILOG2); ++iStage){
			for(ui=0; ui<(M_FOR/2); ++ui){
				Pos = (((L_ID + ui*L_WZ_I)>>iStage)<<iStage) + (L_ID + ui*L_WZ_I);
				LenArrow = 1 << iStage;
				LenArrow = 2*(LenArrow - ((L_ID + ui*L_WZ_I)&(LenArrow-1))) - 1;
				if((Pos+LenArrow+Group) < N_KEYS){
					CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
				}
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			
			#ifdef UNROLL_BSS_LEVEL
			#pragma unroll
			#endif
			for(iLevel=iStage-1; iLevel>=0; --iLevel){
				LenArrow = 1 << iLevel;
				for(ui=0; ui<(M_FOR/2); ++ui){
					Pos = (((L_ID + ui*L_WZ_I)>>iLevel)<<iLevel) + (L_ID + ui*L_WZ_I);
					if((Pos+LenArrow+Group) < N_KEYS){
						CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
					}
				}				
				barrier(CLK_LOCAL_MEM_FENCE);
			}		
		}
		
		Pos = Group + L_ID;
		#ifdef UNROLL_BSS_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			if(Pos+ui*L_WZ_I < N_KEYS){
				sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
			}
		}		
	}	
}


__kernel __attribute__((reqd_work_group_size(L_WZ_I, 1, 1))) void BitonicSortLevel(__global SORTDATA *sortData)
{
	unsigned int L_ID, Pos, LenArrow, ui, Group;
	int iLevel;
	__local SORTDATA sortLocTmp[M_FOR*L_WZ_I];
	
	L_ID = get_local_id(0);
	Group = M_FOR*L_WZ_I*get_group_id(0);
	Pos = Group + L_ID;

	if(Group + M_FOR*L_WZ_I < N_KEYS){
		
		#ifdef UNROLL_BSL_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
		}		
		barrier(CLK_LOCAL_MEM_FENCE);
			
		#ifdef UNROLL_BSL_LEVEL
		#pragma unroll
		#endif
		for(iLevel = (ILOG2-1); iLevel>=0; --iLevel){
			LenArrow = 1 << iLevel;
			for(ui=0; ui<(M_FOR/2); ++ui){
				Pos = (((L_ID + ui*L_WZ_I)>>iLevel)<<iLevel) + (L_ID + ui*L_WZ_I);
				CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
			}	
			barrier(CLK_LOCAL_MEM_FENCE);
		}		

		Pos = Group + L_ID;
		#ifdef UNROLL_BSL_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
		}	

	}
	else{
		
		#ifdef UNROLL_BSL_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			if(Pos+ui*L_WZ_I < N_KEYS){
				sortLocTmp[L_ID + ui*L_WZ_I] = sortData[Pos+ui*L_WZ_I];
			}
		}		
		barrier(CLK_LOCAL_MEM_FENCE);
			
		#ifdef UNROLL_BSL_LEVEL
		#pragma unroll
		#endif
		for(iLevel = (ILOG2-1); iLevel>=0; --iLevel){
			LenArrow = 1 << iLevel;
			for(ui=0; ui<(M_FOR/2); ++ui){
				Pos = (((L_ID + ui*L_WZ_I)>>iLevel)<<iLevel) + (L_ID + ui*L_WZ_I);
				if((Pos+LenArrow+Group) < N_KEYS){
					CmpXchg(sortLocTmp+Pos, sortLocTmp+Pos+LenArrow);
				}
			}	
			barrier(CLK_LOCAL_MEM_FENCE);
		}		

		Pos = Group + L_ID;
		#ifdef UNROLL_BSL_MEMCPY
		#pragma unroll
		#endif	
		for(ui=0; ui<M_FOR; ++ui){
			if(Pos+ui*L_WZ_I < N_KEYS){
				sortData[Pos + ui*L_WZ_I] = sortLocTmp[L_ID + ui*L_WZ_I];
			}
		}	
		
	}
	
}

__kernel __attribute__((reqd_work_group_size(L_WZ, 1, 1))) void BitonicSort(__global SORTDATA *sortData, int iLevel)
{
	unsigned int G_ID, Pos, LenArrow;
	
	G_ID = get_global_id(0);
	
	Pos = ((G_ID>>iLevel)<<iLevel) + G_ID;
	LenArrow = 1 << iLevel;
	
	if((Pos+LenArrow) < N_KEYS){
		CmpXchg(sortData+Pos, sortData+Pos+LenArrow);
	}
}

__kernel __attribute__((reqd_work_group_size(L_WZ, 1, 1))) void BitonicMerge(__global SORTDATA *sortData, int iLevel)
{
	unsigned int G_ID, Pos, LenArrow;
	
	G_ID = get_global_id(0);
	Pos = ((G_ID>>iLevel)<<iLevel) + G_ID;

	LenArrow = 1 << iLevel;
	LenArrow = 2*(LenArrow - (G_ID&(LenArrow-1))) - 1;
	
	if((Pos+LenArrow) < N_KEYS){
		CmpXchg(sortData+Pos, sortData+Pos+LenArrow);
	}
}
