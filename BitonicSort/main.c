#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "OclBitonicSort/OclBitonicsSort.h"


#define CLOCK_GETTIME_AVAILABLE


#if defined(CLOCK_GETTIME_AVAILABLE)
typedef struct timespec PERF_TIMER;
#else
typedef struct timeval PERF_TIMER;
#endif

static inline int PerformanceTimer(PERF_TIMER *ptvs){
#if defined(CLOCK_GETTIME_AVAILABLE)
	return  clock_gettime(CLOCK_MONOTONIC, ptvs);
#else
	return gettimeofday(ptvs, NULL);
#endif
}

int ElapsedTimeSec(PERF_TIMER *pA, PERF_TIMER *pB, double *pdElapSec)
{
	if(pA && pB && pdElapSec){
#if defined(CLOCK_GETTIME_AVAILABLE)
		*pdElapSec = (double)(pB->tv_sec - pA->tv_sec) + (double)(pB->tv_nsec - pA->tv_nsec)*1e-9;
#else
		*pdElapSec = (double)(pB->tv_sec - pA->tv_sec) + (double)(pB->tv_usec - pA->tv_usec)*1e-6;
#endif
		return 0;
	}

	return 1;
}

int ElapsedTimeUsec(PERF_TIMER *pA, PERF_TIMER *pB, unsigned long long *pullElapUsec)
{
	if(pA && pB && pullElapUsec){
#if defined(CLOCK_GETTIME_AVAILABLE)
		*pullElapUsec = (unsigned long long)((pB->tv_sec - pA->tv_sec)*1000000 + (pB->tv_nsec - pA->tv_nsec)/1000);
#else
		*pullElapUsec = (unsigned long long)((pB->tv_sec - pA->tv_sec)*1000000 + (pB->tv_usec - pA->tv_usec));
#endif
		return 0;
	}

	return 1;
}


int Run(void *pToSort, void *pSorted, size_t zLen, ESORTTYPE eSorttype)
{
	OCLTRIM oclTrim;
	cl_platform_id clPlatformID;
	cl_device_id clDeviceID;
	char sTmp[256];

	if(clGetPlatformIDs(1, &clPlatformID, NULL) != CL_SUCCESS){
		return 1;
	}
	if(clGetDeviceIDs(clPlatformID, CL_DEVICE_TYPE_GPU, 1, &clDeviceID , NULL) != CL_SUCCESS){
		return 1;
	}
	if(clGetDeviceInfo(clDeviceID, CL_DEVICE_NAME, sizeof(sTmp)/sizeof(*sTmp), sTmp, NULL) == CL_SUCCESS){
		puts(sTmp);
	}

	oclTrim.zLocalWorkSize = 128;
	oclTrim.zLocalWorkSizeInside = 128;
	oclTrim.bUnrollBslMemcpy = 1;
	oclTrim.bUnrollBslLevel = 0;
	oclTrim.bUnrollBssLevel = 0;
	oclTrim.bUnrollBssMemcpy = 1;
	oclTrim.bUnrollBssStage = 0;
	oclTrim.iL = BS_L8;
	oclTrim.bShift = 1;

	return OclBitonicSort(clPlatformID, clDeviceID, &oclTrim, pToSort, pSorted, zLen, eSorttype);
}




int main(int argc, char *argv[])
{
	int iRet;
	PERF_TIMER tStart = {0}, tEnd = {0};
	unsigned long long ull;
	int32_t *piDataToSort;
	int32_t *piDataSorted;
	size_t z, zLen = 16384;

	piDataToSort = malloc(zLen * sizeof(piDataToSort[0]));
	if(!piDataToSort){
		return 1;
	}

	piDataSorted = calloc(zLen, sizeof(piDataSorted[0]));
	if(!piDataToSort){
		free(piDataToSort);
		return 1;
	}

	srand(542);


	for(z=0; z<zLen; ++z){
		piDataToSort[z] = rand();
	}

	PerformanceTimer(&tStart);
	iRet = Run(piDataToSort, piDataSorted, zLen, SORTTYPE_INT32);
	PerformanceTimer(&tEnd);
	printf("Run return: %d\n", iRet);
	ElapsedTimeUsec(&tStart, &tEnd, &ull);
	printf("Timer: %llu us\n", ull);

	PerformanceTimer(&tStart);
	iRet = Run(piDataToSort, piDataSorted, zLen, SORTTYPE_INT32);
	PerformanceTimer(&tEnd);
	printf("Run return: %d\n", iRet);
	ElapsedTimeUsec(&tStart, &tEnd, &ull);
	printf("Timer: %llu us\n", ull);

	PerformanceTimer(&tStart);
	iRet = Run(piDataToSort, piDataSorted, zLen, SORTTYPE_INT32);
	PerformanceTimer(&tEnd);
	printf("Run return: %d\n", iRet);
	ElapsedTimeUsec(&tStart, &tEnd, &ull);
	printf("Timer: %llu us\n", ull);

	for(z=0; z<4; ++z){
		printf("%d ", piDataToSort[z]);
	}
	putchar('\n');
	for(z=0; z<4; ++z){
		printf("%d ", piDataSorted[z]);
	}
	putchar('\n');


	free(piDataSorted);
	free(piDataToSort);

	puts("Done.");
	return 0;
}
