#ifndef OCLBITONICSORT_OCLBITONICSSORT_H_
#define OCLBITONICSORT_OCLBITONICSSORT_H_


#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/cl.h"
#endif


enum _SORTTYPES{
	SORTTYPE_NULL = 0,

	SORTTYPE_INT8,
	SORTTYPE_INT16,
	SORTTYPE_INT32,
	SORTTYPE_INT64,

	SORTTYPE_UINT8,
	SORTTYPE_UINT16,
	SORTTYPE_UINT32,
	SORTTYPE_UINT64,

	SORTTYPE_FLOAT,
	SORTTYPE_DOUBLE,
};

typedef enum _SORTTYPES ESORTTYPE;

#define BS_L1 1
#define BS_L2 2
#define BS_L4 4
#define BS_L8 8
#define BS_L16 16
#define BS_L32 32
#define BS_L64 64
#define BS_L128 128


typedef struct tagOCLTRIM{
	size_t zLocalWorkSize;
	size_t zLocalWorkSizeInside;
	_Bool bUnrollBssMemcpy;
	_Bool bUnrollBssStage;
	_Bool bUnrollBssLevel;
	_Bool bUnrollBslMemcpy;
	_Bool bUnrollBslLevel;
	int iL;
	_Bool bShift;
}OCLTRIM;

typedef struct tagOCLAPPDATA{
	cl_platform_id clPlatform;
	cl_device_id clDevice;
	cl_context clContext;
	cl_command_queue clCmdQueue;
	cl_mem clMem;
	size_t zN;
	ESORTTYPE eSorttype;
	OCLTRIM *pOclTrim;
}OCLAPPDATA;

typedef struct tagOCLBS{
	cl_program clProg;
	cl_kernel clKernelSort;
	cl_kernel clKernelMerge;
	cl_kernel clKernelSortLevel;
	cl_kernel clKernelSortStage;
}OCLBS;

int OclBitonicSort(cl_platform_id, cl_device_id, OCLTRIM* , void *, void *, size_t, ESORTTYPE);


#endif /* OCLBITONICSORT_OCLBITONICSSORT_H_ */
