#define MAX_STRING_LEN 64

inline int StringCmp(__global char *sA, __global char *sB, int iMax)
{
	int iCmp;
	char cA, cB;
	
	while(iMax > 0){
		cA = *sA;
		cB = *sB;
		iCmp = (int)(cA) - (int)(cB);

		if(iCmp || cA == '\0' || cB == '\0'){
			return iCmp;
		}

		++sA;
		++sB;
		--iMax;
	}

	return 0;
}


__kernel void InitData(__global unsigned int *sort)
{
	unsigned int G_ID;
	
	G_ID = get_global_id(0);
	
	sort[G_ID] = G_ID;
}



__kernel void BitonicSort(__global char *data, __global unsigned int *offset, __global unsigned int *sort, int level)
{
	unsigned int G_ID, Pos, LenArrow;
	unsigned int uiTmp;
	
	G_ID = get_global_id(0);
	
	Pos = ((G_ID>>level)<<level) + G_ID;
	LenArrow = 1 << level;
	
	if(StringCmp(data + offset[sort[Pos]], data + offset[sort[Pos+LenArrow]], MAX_STRING_LEN ) > 0){
		uiTmp = sort[Pos];
		sort[Pos] = sort[Pos+LenArrow];
		sort[Pos+LenArrow] = uiTmp;
	}
	
}

__kernel void BitonicMerge(__global char *data, __global unsigned int *offset, __global unsigned int *sort, int level)
{
	unsigned int G_ID, Pos, LenArrow;
	unsigned int uiTmp;
	
	G_ID = get_global_id(0);
	Pos = ((G_ID>>level)<<level) + G_ID;

	LenArrow = 1 << level;
	LenArrow = 2*(LenArrow - (G_ID&(LenArrow-1))) - 1;

	if(StringCmp(data + offset[sort[Pos]], data + offset[sort[Pos+LenArrow]], MAX_STRING_LEN ) > 0){
		uiTmp = sort[Pos];
		sort[Pos] = sort[Pos+LenArrow];
		sort[Pos+LenArrow] = uiTmp;
	}
}