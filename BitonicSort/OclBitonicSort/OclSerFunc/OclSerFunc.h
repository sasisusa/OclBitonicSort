#ifndef OCLSERFUNC_H_
#define OCLSERFUNC_H_

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/cl.h"
#endif

#include <string.h>

#if !defined(ZeroMemory)
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif

_Bool IsNotPowerOfTwo(size_t);
size_t CeilToPow2(size_t);
size_t FloorToPow2(size_t);
size_t CeilToLog2(size_t);
size_t FloorToLog2(size_t);

const char *OclGetDevTypeString(cl_device_type *);
const char *OclErrorCodeToString(cl_int);
const char *OclPlatformInfoToString(cl_platform_info);
const char *OclDevieInfoToString(cl_device_info);

size_t OclGetMaxWorkgroupSize(cl_device_id *);
int OclIsLocalMemLocal(cl_device_id *);
cl_ulong OclGetLocalMemSize(cl_device_id *);


int FreeAllocSpace(void *);
char *GetFileStringContent(const char *, size_t *);

#endif /* OCLSERFUNC_H_ */
