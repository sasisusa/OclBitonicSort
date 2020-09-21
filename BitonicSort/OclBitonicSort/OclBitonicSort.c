#include <stdio.h>

#include "OclBitonicsSort.h"
#include "OclSerFunc/OclSerFunc.h"


int OclReleaseAppData(OCLAPPDATA *pOclAppData)
{
	cl_int iErr = 0;

	if(pOclAppData->clMem){
		iErr |= clReleaseMemObject(pOclAppData->clMem);
		pOclAppData->clMem = NULL;
	}
	if(pOclAppData->clCmdQueue){
		iErr |= clReleaseCommandQueue(pOclAppData->clCmdQueue);
		pOclAppData->clCmdQueue = NULL;
	}
	if(pOclAppData->clContext){
		iErr |= clReleaseContext(pOclAppData->clContext);
		pOclAppData->clContext = NULL;
	}

	return iErr;
}


int OclReleaseBS(OCLBS *pOclBS)
{
	cl_int iRet = 0;


	if(pOclBS->clKernelMerge){
		iRet |= clReleaseKernel(pOclBS->clKernelMerge);
		pOclBS->clKernelMerge = NULL;
	}
	if(pOclBS->clKernelSort){
		iRet |= clReleaseKernel(pOclBS->clKernelSort);
		pOclBS->clKernelSort = NULL;
	}
	if(pOclBS->clKernelSortLevel){
		iRet |= clReleaseKernel(pOclBS->clKernelSortLevel);
		pOclBS->clKernelSortLevel = NULL;
	}
	if(pOclBS->clKernelSortStage){
		iRet |= clReleaseKernel(pOclBS->clKernelSortStage);
		pOclBS->clKernelSortStage = NULL;
	}
	if(pOclBS->clProg){
		iRet |= clReleaseProgram(pOclBS->clProg);
		pOclBS->clProg = NULL;
	}

	return iRet;
}


size_t SizeOfeSorttype(int eSorttype)
{
	size_t zSize = 0;

	switch(eSorttype){
	case SORTTYPE_INT8:
		zSize = sizeof(cl_char);
		break;
	case SORTTYPE_UINT8:
		zSize = sizeof(cl_uchar);
		break;
	case SORTTYPE_INT16:
		zSize = sizeof(cl_short);
		break;
	case SORTTYPE_UINT16:
		zSize = sizeof(cl_ushort);
		break;
	case SORTTYPE_INT32:
		zSize = sizeof(cl_int);
		break;
	case SORTTYPE_UINT32:
		zSize = sizeof(cl_uint);
		break;
	case SORTTYPE_FLOAT:
		zSize = sizeof(cl_float);
		break;
	case SORTTYPE_INT64:
		zSize = sizeof(cl_long);
		break;
	case SORTTYPE_UINT64:
		zSize = sizeof(cl_ulong);
		break;
	case SORTTYPE_DOUBLE:
		zSize = sizeof(cl_double);
		break;
	}

	return zSize;
}


const char * ClTypeOfeSorttype(int eSorttype)
{
	const char *sRet = NULL;

	switch(eSorttype){
	case SORTTYPE_INT8:
		sRet = "char";
		break;
	case SORTTYPE_UINT8:
		sRet = "uchar";
		break;
	case SORTTYPE_INT16:
		sRet = "short";
		break;
	case SORTTYPE_UINT16:
		sRet = "ushort";
		break;
	case SORTTYPE_INT32:
		sRet = "int";
		break;
	case SORTTYPE_UINT32:
		sRet = "uint";
		break;
	case SORTTYPE_FLOAT:
		sRet = "float";
		break;
	case SORTTYPE_INT64:
		sRet = "long";
		break;
	case SORTTYPE_UINT64:
		sRet = "ulong";
		break;
	case SORTTYPE_DOUBLE:
		sRet = "double";
		break;
	}

	return sRet;
}


int OclRunBitonicSortPow2(OCLAPPDATA *pOclAppData, char *sKernelFile)
{
	char *sKernelSrc;
	OCLBS oclBS;
	cl_int iErr, iStage, iLevel, iLog2;
	cl_uint uiStage, uiLevel, uiNShift, uiFor;
	size_t z, zGlobalWorkSize, zLocalWorkSizeInside, zLocalWorkSize;
	char sBuf[512];

	ZeroMemory(&oclBS, sizeof(oclBS));

	sKernelSrc = GetFileStringContent(sKernelFile, NULL);
	if(!sKernelSrc){
		puts("Error: GetFileStringContent");
		return 1;
	}

	oclBS.clProg = clCreateProgramWithSource(pOclAppData->clContext, 1, (const char **)&sKernelSrc, NULL, &iErr);
	FreeAllocSpace(sKernelSrc);
	if(iErr){
		puts("Error: clCreateProgramWithSource");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	uiFor = pOclAppData->pOclTrim->iL;
	uiNShift = FloorToLog2(uiFor);


	zLocalWorkSizeInside = pOclAppData->pOclTrim->zLocalWorkSizeInside;
	zLocalWorkSize = pOclAppData->pOclTrim->zLocalWorkSize;

	iErr = snprintf(sBuf, sizeof(sBuf)/sizeof(*sBuf), "-D SORTTYPE=%s -D N_KEYS=%lu -D L_WZ=%lu -D L_WZ_I=%lu -D ILOG2=%u -D M_FOR=%u %s%s%s%s%s",
			ClTypeOfeSorttype(pOclAppData->eSorttype), pOclAppData->zN, zLocalWorkSize, zLocalWorkSizeInside,
			(unsigned int)FloorToLog2(zLocalWorkSizeInside)+uiNShift, uiFor,
			(pOclAppData->pOclTrim->bUnrollBssMemcpy)?"-D UNROLL_BSS_MEMCPY ":"",
			(pOclAppData->pOclTrim->bUnrollBssStage)?"-D UNROLL_BSS_STAGE ":"",
			(pOclAppData->pOclTrim->bUnrollBssLevel)?"-D UNROLL_BSS_LEVEL ":"",
			(pOclAppData->pOclTrim->bUnrollBslMemcpy)?"-D UNROLL_BSL_MEMCPY ":"",
			(pOclAppData->pOclTrim->bUnrollBslLevel)?"-D UNROLL_BSL_LEVEL ":""
	);
	if(iErr < 0 || iErr > sizeof(sBuf)/sizeof(*sBuf)){
		puts("Error: snprintf");
		OclReleaseBS(&oclBS);
		return 1;
	}

	iErr = clBuildProgram(oclBS.clProg, 1, &pOclAppData->clDevice, sBuf, NULL, NULL);
	if(iErr){
		char *sBuildLog;
		puts("Error: clBuildProgram");

		clGetProgramBuildInfo(oclBS.clProg, pOclAppData->clDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &z);
		sBuildLog = malloc(z);
		if(sBuildLog){
			clGetProgramBuildInfo(oclBS.clProg, pOclAppData->clDevice, CL_PROGRAM_BUILD_LOG, z, sBuildLog, NULL);
			puts(sBuildLog);
			free(sBuildLog);
		}
		OclReleaseBS(&oclBS);
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	oclBS.clKernelMerge = clCreateKernel(oclBS.clProg, "BitonicMerge", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicMerge");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSort = clCreateKernel(oclBS.clProg, "BitonicSort", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSort");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSortLevel = clCreateKernel(oclBS.clProg, "BitonicSortLevel", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSortLevel");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSortStage = clCreateKernel(oclBS.clProg, "BitonicSortStage", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSortStage");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	zGlobalWorkSize = pOclAppData->zN >> 1;
	iErr = clSetKernelArg(oclBS.clKernelMerge, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSort, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSortLevel, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSortStage, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);

	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clSetKernelArg, first argument");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	if(pOclAppData->pOclTrim->bShift){
		iLog2 = FloorToLog2(pOclAppData->zN);
		iStage = FloorToLog2(zLocalWorkSizeInside) + uiNShift;
		zGlobalWorkSize = pOclAppData->zN >> uiNShift;

		clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortStage, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
		zGlobalWorkSize = pOclAppData->zN >> 1;
		for(; iStage<iLog2; ++iStage){
			clSetKernelArg(oclBS.clKernelMerge, 1, sizeof(iStage), &iStage);
			clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelMerge, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
			for(iLevel=iStage-1; iLevel>=0; --iLevel){
				if( (1<<iLevel) > (1<<(uiNShift-1))*(zLocalWorkSizeInside) ){
					clSetKernelArg(oclBS.clKernelSort, 1, sizeof(iLevel), &iLevel);
					clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSort, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
				}
				else{
					zGlobalWorkSize = pOclAppData->zN >> uiNShift;
					clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortLevel, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
					zGlobalWorkSize = pOclAppData->zN >> 1;
					break;
				}
			}
		}
	}
	else{
		uiStage = (1<<uiNShift)*(zLocalWorkSizeInside);
		zGlobalWorkSize = pOclAppData->zN >> uiNShift;

		clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortStage, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
		zGlobalWorkSize = pOclAppData->zN >> 1;
		for(; uiStage<pOclAppData->zN; uiStage*=2){
			clSetKernelArg(oclBS.clKernelMerge, 1, sizeof(uiStage), &uiStage);
			clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelMerge, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
			for(uiLevel=uiStage>>1; uiLevel>0; uiLevel>>=1){
				if( uiLevel > (1<<(uiNShift-1))*(zLocalWorkSizeInside) ){
					clSetKernelArg(oclBS.clKernelSort, 1, sizeof(uiLevel), &uiLevel);
					clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSort, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
				}
				else{
					zGlobalWorkSize = pOclAppData->zN >> uiNShift;
					clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortLevel, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
					zGlobalWorkSize = pOclAppData->zN >> 1;
					break;
				}
			}
		}
	}


	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clEnqueueNDRangeKernel | clSetKernelArg");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	iErr = clFinish(pOclAppData->clCmdQueue);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clFinish");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	iErr = OclReleaseBS(&oclBS);
	if(iErr){
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	return 0;
}



int OclRunBitonicSortArb(OCLAPPDATA *pOclAppData, char *sKernelFile)
{
	char *sKernelSrc;
	OCLBS oclBS;
	cl_int iErr, iStage, iLevel, iLog2;
	cl_uint uiStage, uiLevel, uiNShift, uiFor;
	size_t z, zGlobalWorkSize, zLocalWorkSizeInside, zLocalWorkSize;
	char sBuf[512];

	ZeroMemory(&oclBS, sizeof(oclBS));

	sKernelSrc = GetFileStringContent(sKernelFile, NULL);
	if(!sKernelSrc){
		puts("Error: GetFileStringContent");
		return 1;
	}

	oclBS.clProg = clCreateProgramWithSource(pOclAppData->clContext, 1, (const char **)&sKernelSrc, NULL, &iErr);
	FreeAllocSpace(sKernelSrc);
	if(iErr){
		puts("Error: clCreateProgramWithSource");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	uiFor = pOclAppData->pOclTrim->iL;
	uiNShift = FloorToLog2(uiFor);


	zLocalWorkSizeInside = pOclAppData->pOclTrim->zLocalWorkSizeInside;
	zLocalWorkSize = pOclAppData->pOclTrim->zLocalWorkSize;

	iErr = snprintf(sBuf, sizeof(sBuf)/sizeof(*sBuf), "-D SORTTYPE=%s -D N_KEYS=%lu -D L_WZ=%lu -D L_WZ_I=%lu -D ILOG2=%u -D M_FOR=%u %s%s%s%s%s",
			ClTypeOfeSorttype(pOclAppData->eSorttype), pOclAppData->zN, zLocalWorkSize, zLocalWorkSizeInside,
			(unsigned int)FloorToLog2(zLocalWorkSizeInside)+uiNShift, uiFor,
			(pOclAppData->pOclTrim->bUnrollBssMemcpy)?"-D UNROLL_BSS_MEMCPY ":"",
			(pOclAppData->pOclTrim->bUnrollBssStage)?"-D UNROLL_BSS_STAGE ":"",
			(pOclAppData->pOclTrim->bUnrollBssLevel)?"-D UNROLL_BSS_LEVEL ":"",
			(pOclAppData->pOclTrim->bUnrollBslMemcpy)?"-D UNROLL_BSL_MEMCPY ":"",
			(pOclAppData->pOclTrim->bUnrollBslLevel)?"-D UNROLL_BSL_LEVEL ":""
	);
	if(iErr < 0 || iErr > sizeof(sBuf)/sizeof(*sBuf)){
		puts("Error: snprintf");
		OclReleaseBS(&oclBS);
		return 1;
	}

	iErr = clBuildProgram(oclBS.clProg, 1, &pOclAppData->clDevice, sBuf, NULL, NULL);
	if(iErr){
		char *sBuildLog;
		puts("Error: clBuildProgram");

		clGetProgramBuildInfo(oclBS.clProg, pOclAppData->clDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &z);
		sBuildLog = malloc(z);
		if(sBuildLog){
			clGetProgramBuildInfo(oclBS.clProg, pOclAppData->clDevice, CL_PROGRAM_BUILD_LOG, z, sBuildLog, NULL);
			puts(sBuildLog);
			free(sBuildLog);
		}
		OclReleaseBS(&oclBS);
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	oclBS.clKernelMerge = clCreateKernel(oclBS.clProg, "BitonicMerge", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicMerge");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSort = clCreateKernel(oclBS.clProg, "BitonicSort", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSort");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSortLevel = clCreateKernel(oclBS.clProg, "BitonicSortLevel", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSortLevel");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}
	oclBS.clKernelSortStage = clCreateKernel(oclBS.clProg, "BitonicSortStage", &iErr);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clCreateKernel, BitonicSortStage");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	zGlobalWorkSize = pOclAppData->zN >> 1;
	iErr = clSetKernelArg(oclBS.clKernelMerge, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSort, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSortLevel, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);
	iErr |= clSetKernelArg(oclBS.clKernelSortStage, 0, sizeof(pOclAppData->clMem), &pOclAppData->clMem);

	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clSetKernelArg, first argument");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	if(pOclAppData->pOclTrim->bShift){
		iLog2 = CeilToLog2(pOclAppData->zN);
		iStage = FloorToLog2(zLocalWorkSizeInside) + uiNShift;

		size_t zGlobalWorkSizeSortLevel = ((((pOclAppData->zN + 1) >> uiNShift) + (zLocalWorkSizeInside)-1)/(zLocalWorkSizeInside))*(zLocalWorkSizeInside);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>uiNShift;
		if(zGlobalWorkSizeSortLevel > zGlobalWorkSize){
			zGlobalWorkSizeSortLevel = zGlobalWorkSize;
			if(zLocalWorkSizeInside > zGlobalWorkSizeSortLevel){
				zLocalWorkSizeInside = zGlobalWorkSizeSortLevel;
			}
		}

		size_t zGlobalWorkSizeSort = ((((pOclAppData->zN + 1) >> 1) + (zLocalWorkSize)-1)/(zLocalWorkSize))*(zLocalWorkSize);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>1;
		if(zGlobalWorkSizeSort > zGlobalWorkSize){
			zGlobalWorkSizeSort = zGlobalWorkSize;
			if(zLocalWorkSize > zGlobalWorkSizeSort){
				zLocalWorkSize = zGlobalWorkSizeSort;
			}
		}
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>uiNShift;

		iErr = clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortStage, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>1;
		for(; iStage<iLog2; ++iStage){
			iErr |= clSetKernelArg(oclBS.clKernelMerge, 1, sizeof(iStage), &iStage);
			iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelMerge, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
			if(iErr){
				break;
			}
			for(iLevel=iStage-1; iLevel>=0; --iLevel){
				if( (1<<iLevel) > (1<<(uiNShift-1))*(zLocalWorkSizeInside) ){
					iErr |= clSetKernelArg(oclBS.clKernelSort, 1, sizeof(iLevel), &iLevel);
					iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSort, 1, NULL, &zGlobalWorkSizeSort, &zLocalWorkSize, 0 , NULL, NULL);
				}
				else{
					iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortLevel, 1, NULL, &zGlobalWorkSizeSortLevel, &zLocalWorkSizeInside, 0 , NULL, NULL);
					break;
				}
			}
		}
	}
	else{
		uiStage = (1<<uiNShift)*(zLocalWorkSizeInside);

		size_t zGlobalWorkSizeSortLevel = ((((pOclAppData->zN + 1) >> uiNShift) + (zLocalWorkSizeInside)-1)/(zLocalWorkSizeInside))*(zLocalWorkSizeInside);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>uiNShift;
		if(zGlobalWorkSizeSortLevel > zGlobalWorkSize){
			zGlobalWorkSizeSortLevel = zGlobalWorkSize;
			if(zLocalWorkSizeInside > zGlobalWorkSizeSortLevel){
				zLocalWorkSizeInside = zGlobalWorkSizeSortLevel;
			}
		}

		size_t zGlobalWorkSizeSort = ((((pOclAppData->zN + 1) >> 1) + (zLocalWorkSize)-1)/(zLocalWorkSize))*(zLocalWorkSize);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>1;
		if(zGlobalWorkSizeSort > zGlobalWorkSize){
			zGlobalWorkSizeSort = zGlobalWorkSize;
			if(zLocalWorkSize > zGlobalWorkSizeSort){
				zLocalWorkSize = zGlobalWorkSizeSort;
			}
		}
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>uiNShift;

		iErr = clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortStage, 1, NULL, &zGlobalWorkSize, &zLocalWorkSizeInside, 0 , NULL, NULL);
		zGlobalWorkSize = CeilToPow2(pOclAppData->zN)>>1;
		for(; uiStage<pOclAppData->zN; uiStage*=2){
			iErr |= clSetKernelArg(oclBS.clKernelMerge, 1, sizeof(uiStage), &uiStage);
			iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelMerge, 1, NULL, &zGlobalWorkSize, &zLocalWorkSize, 0 , NULL, NULL);
			if(iErr){
				break;
			}
			for(uiLevel=uiStage>>1; uiLevel>0; uiLevel>>=1){
				if( uiLevel > (1<<(uiNShift-1))*(zLocalWorkSizeInside) ){
					iErr |= clSetKernelArg(oclBS.clKernelSort, 1, sizeof(uiLevel), &uiLevel);
					iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSort, 1, NULL, &zGlobalWorkSizeSort, &zLocalWorkSize, 0 , NULL, NULL);
				}
				else{
					iErr |= clEnqueueNDRangeKernel(pOclAppData->clCmdQueue, oclBS.clKernelSortLevel, 1, NULL, &zGlobalWorkSizeSortLevel, &zLocalWorkSizeInside, 0 , NULL, NULL);
					break;
				}
			}
		}
	}


	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clEnqueueNDRangeKernel | clSetKernelArg");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	iErr = clFinish(pOclAppData->clCmdQueue);
	if(iErr){
		OclReleaseBS(&oclBS);
		puts("Error: clFinish");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	iErr = OclReleaseBS(&oclBS);
	if(iErr){
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	return 0;
}




int OclBitonicSort(cl_platform_id clPlatformID, cl_device_id clDeviceID, OCLTRIM *pOclTrim, void *pToSort, void *pSorted, size_t zLen, ESORTTYPE eSorttype)
{
	cl_int iErr;
	cl_ulong ulLocMem;
	size_t zMaxWGZ;
	OCLAPPDATA oclAppData;

	ZeroMemory(&oclAppData, sizeof(oclAppData));
	oclAppData.clPlatform = clPlatformID;
	oclAppData.clDevice = clDeviceID;
	oclAppData.zN = zLen;
	oclAppData.pOclTrim = pOclTrim;
	oclAppData.eSorttype = eSorttype;

	ulLocMem = OclGetLocalMemSize(&oclAppData.clDevice);
	zMaxWGZ = OclGetMaxWorkgroupSize(&oclAppData.clDevice);
	if(ulLocMem == 0 || zMaxWGZ == 0){
		return 1;
	}

	if(oclAppData.zN < pOclTrim->iL*pOclTrim->zLocalWorkSizeInside || pOclTrim->iL*pOclTrim->zLocalWorkSizeInside*SizeOfeSorttype(oclAppData.eSorttype) > ulLocMem){
		puts("Error: Conflict with trimming parameters");
		return 1;
	}

	oclAppData.clContext = clCreateContext((cl_context_properties[]){CL_CONTEXT_PLATFORM, (cl_context_properties)oclAppData.clPlatform, 0},
								1, &oclAppData.clDevice, NULL, NULL, &iErr);
	if(iErr){
		OclReleaseAppData(&oclAppData);
		puts("Error: clCreateContext");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	oclAppData.clCmdQueue = clCreateCommandQueue(oclAppData.clContext, oclAppData.clDevice, 0, &iErr);
	if(iErr){
		OclReleaseAppData(&oclAppData);
		puts("Error: clCreateCommandQueue");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	oclAppData.clMem = clCreateBuffer(oclAppData.clContext, CL_MEM_READ_WRITE, oclAppData.zN * SizeOfeSorttype(oclAppData.eSorttype), NULL, &iErr);
	if(iErr){
		OclReleaseAppData(&oclAppData);
		puts("Error: clCreateBuffer");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	iErr = clEnqueueWriteBuffer(oclAppData.clCmdQueue, oclAppData.clMem, CL_TRUE, 0, oclAppData.zN * SizeOfeSorttype(oclAppData.eSorttype), pToSort, 0, NULL, NULL);
	if(iErr){
		OclReleaseAppData(&oclAppData);
		puts("Error: clEnqueueWriteBuffer");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}


	if(IsNotPowerOfTwo(oclAppData.zN)){
		if(oclAppData.pOclTrim->bShift){
			iErr = OclRunBitonicSortArb(&oclAppData, "./OclBitonicSort/CL_BS_Kernel/ARB/BSM_LocM_PosShift_DWZ_Unroll_FixL_For.cl");
		}
		else{
			iErr = OclRunBitonicSortArb(&oclAppData, "./OclBitonicSort/CL_BS_Kernel/ARB/BSM_LocM_PosAnd_DWZ_Unroll_FixL_For.cl");
		}
		if(iErr){
			OclReleaseAppData(&oclAppData);
			puts("Error: OclRunBitonicSortArb");
			return 1;
		}
	}
	else{
		puts("Pow2");
		if(oclAppData.pOclTrim->bShift){
			iErr = OclRunBitonicSortArb(&oclAppData, "./OclBitonicSort/CL_BS_Kernel/P2/BSM_LocM_PosShift_DWZ_Unroll_FixL_For.cl");
		}
		else{
			iErr = OclRunBitonicSortArb(&oclAppData, "./OclBitonicSort/CL_BS_Kernel/P2/BSM_LocM_PosAnd_DWZ_Unroll_FixL_For.cl");
		}
		if(iErr){
			OclReleaseAppData(&oclAppData);
			puts("Error: OclRunBitonicSortArb");
			return 1;
		}
	}


	iErr = clEnqueueReadBuffer(oclAppData.clCmdQueue, oclAppData.clMem, CL_TRUE, 0, oclAppData.zN * SizeOfeSorttype(oclAppData.eSorttype), pSorted, 0, NULL, NULL);
	if(iErr){
		OclReleaseAppData(&oclAppData);
		puts("Error: clEnqueueReadBuffer");
		puts(OclErrorCodeToString(iErr));
		return 1;
	}

	OclReleaseAppData(&oclAppData);


	return 0;
}
