#include "OclSerFunc.h"

#include <stdio.h>

_Bool IsNotPowerOfTwo(size_t z)
{
	return z ? (z&(z-1)) : 1;
}


////////////////////////////////////////////////////////////////////////////
//	CeilToPow2
//
//	This function returns the nearest upwards rounded power of two of z.
//	255 -> 256, 256 -> 256, 257 -> 512
//
size_t CeilToPow2(size_t z)
{
	size_t zT = 1;

	while(zT < z){
		zT <<= 1;
	}

	return zT;
}

////////////////////////////////////////////////////////////////////////////
//	FloorToPow2
//
//	This function returns the nearest downwards rounded power of two of z.
//	255 -> 128, 256 -> 256, 257 -> 256
//
size_t FloorToPow2(size_t z)
{
	size_t zT = 1;

	while(zT < z){
		zT <<= 1;
	}

	return zT == z ? zT : zT >> 1;
}


////////////////////////////////////////////////////////////////////////////
//	CeilToLog2
//
//	This function returns the upwards rounded base-2 logarithm of z
//	255 -> 8, 256 -> 8, 257 -> 9
//
size_t CeilToLog2(size_t z)
{
	size_t zL = 0, zI = z;

	while(zI > 1){
		zI >>= 1;
		++zL;
	}

	return 1 << zL == z ? zL : zL + 1;
}


////////////////////////////////////////////////////////////////////////////
//	FloorToLog2
//
//	This function returns the downwards rounded base-2 logarithm of z
//	255 -> 7, 256 -> 8, 257 -> 8
//
size_t FloorToLog2(size_t z)
{
	size_t zL = 0;

	while(z > 1){
		z >>= 1;
		++zL;
	}

	return zL;
}


const char *OclGetDevTypeString(cl_device_type *pclDevType)
{
	const char *sRet = NULL;

	if(pclDevType){
		switch(*pclDevType){
		case CL_DEVICE_TYPE_DEFAULT:
			sRet = "DEFAULT";
			break;
		case CL_DEVICE_TYPE_CPU:
			sRet = "CPU";
			break;
		case CL_DEVICE_TYPE_GPU:
			sRet = "GPU";
			break;
		case CL_DEVICE_TYPE_ACCELERATOR:
			sRet = "ACCELERATOR";
			break;
		case CL_DEVICE_TYPE_CUSTOM:
			sRet = "CUSTOM";
			break;
		}
	}

return sRet;
}


const char *OclErrorCodeToString(cl_int iErr)
{
	const char *sRet;

	switch(iErr){
	case CL_SUCCESS:
		sRet = "CL_SUCCESS";
		break;
	case CL_DEVICE_NOT_FOUND:
		sRet = "CL_DEVICE_NOT_FOUND";
		break;
	case CL_DEVICE_NOT_AVAILABLE:
		sRet = "CL_DEVICE_NOT_AVAILABLE";
		break;
	case CL_COMPILER_NOT_AVAILABLE:
		sRet = "CL_COMPILER_NOT_AVAILABLE";
		break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		sRet = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		break;
	case CL_OUT_OF_RESOURCES:
		sRet = "CL_OUT_OF_RESOURCES";
		break;
	case CL_OUT_OF_HOST_MEMORY:
		sRet = "CL_OUT_OF_HOST_MEMORY";
		break;
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		sRet = "CL_PROFILING_INFO_NOT_AVAILABLE";
		break;
	case CL_MEM_COPY_OVERLAP:
		sRet = "CL_MEM_COPY_OVERLAP";
		break;
	case CL_IMAGE_FORMAT_MISMATCH:
		sRet = "CL_IMAGE_FORMAT_MISMATCH";
		break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		sRet = "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		break;
	case CL_BUILD_PROGRAM_FAILURE:
		sRet = "CL_BUILD_PROGRAM_FAILURE";
		break;
	case CL_MAP_FAILURE:
		sRet = "CL_MAP_FAILURE";
		break;
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		sRet = "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		break;
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		sRet = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		break;
	case CL_COMPILE_PROGRAM_FAILURE:
		sRet = "CL_COMPILE_PROGRAM_FAILURE";
		break;
	case CL_LINKER_NOT_AVAILABLE:
		sRet = "CL_LINKER_NOT_AVAILABLE";
		break;
	case CL_LINK_PROGRAM_FAILURE:
		sRet = "CL_LINK_PROGRAM_FAILURE";
		break;
	case CL_DEVICE_PARTITION_FAILED:
		sRet = "CL_DEVICE_PARTITION_FAILED";
		break;
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
		sRet = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
		break;
	case CL_INVALID_VALUE:
		sRet = "CL_INVALID_VALUE";
		break;
	case CL_INVALID_DEVICE_TYPE:
		sRet = "CL_INVALID_DEVICE_TYPE";
		break;
	case CL_INVALID_PLATFORM:
		sRet = "CL_INVALID_PLATFORM";
		break;
	case CL_INVALID_DEVICE:
		sRet = "CL_INVALID_DEVICE";
		break;
	case CL_INVALID_CONTEXT:
		sRet = "CL_INVALID_CONTEXT";
		break;
	case CL_INVALID_QUEUE_PROPERTIES:
		sRet = "CL_INVALID_QUEUE_PROPERTIES";
		break;
	case CL_INVALID_COMMAND_QUEUE:
		sRet = "CL_INVALID_COMMAND_QUEUE";
		break;
	case CL_INVALID_HOST_PTR:
		sRet = "CL_INVALID_HOST_PTR";
		break;
	case CL_INVALID_MEM_OBJECT:
		sRet = "CL_INVALID_MEM_OBJECT";
		break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		sRet = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		break;
	case CL_INVALID_IMAGE_SIZE:
		sRet = "CL_INVALID_IMAGE_SIZE";
		break;
	case CL_INVALID_SAMPLER:
		sRet = "CL_INVALID_SAMPLER";
		break;
	case CL_INVALID_BINARY:
		sRet = "CL_INVALID_BINARY";
		break;
	case CL_INVALID_BUILD_OPTIONS:
		sRet = "CL_INVALID_BUILD_OPTIONS";
		break;
	case CL_INVALID_PROGRAM:
		sRet = "CL_INVALID_PROGRAM";
		break;
	case CL_INVALID_PROGRAM_EXECUTABLE:
		sRet = "CL_INVALID_PROGRAM_EXECUTABLE";
		break;
	case CL_INVALID_KERNEL_NAME:
		sRet = "CL_INVALID_KERNEL_NAME";
		break;
	case CL_INVALID_KERNEL_DEFINITION:
		sRet = "CL_INVALID_KERNEL_DEFINITION";
		break;
	case CL_INVALID_KERNEL:
		sRet = "CL_INVALID_KERNEL";
		break;
	case CL_INVALID_ARG_INDEX:
		sRet = "CL_INVALID_ARG_INDEX";
		break;
	case CL_INVALID_ARG_VALUE:
		sRet = "CL_INVALID_ARG_VALUE";
		break;
	case CL_INVALID_ARG_SIZE:
		sRet = "CL_INVALID_ARG_SIZE";
		break;
	case CL_INVALID_KERNEL_ARGS:
		sRet = "CL_INVALID_KERNEL_ARGS";
		break;
	case CL_INVALID_WORK_DIMENSION:
		sRet = "CL_INVALID_WORK_DIMENSION";
		break;
	case CL_INVALID_WORK_GROUP_SIZE:
		sRet = "CL_INVALID_WORK_GROUP_SIZE";
		break;
	case CL_INVALID_WORK_ITEM_SIZE:
		sRet = "CL_INVALID_WORK_ITEM_SIZE";
		break;
	case CL_INVALID_GLOBAL_OFFSET:
		sRet = "CL_INVALID_GLOBAL_OFFSET";
		break;
	case CL_INVALID_EVENT_WAIT_LIST:
		sRet = "CL_INVALID_EVENT_WAIT_LIST";
		break;
	case CL_INVALID_EVENT:
		sRet = "CL_INVALID_EVENT";
		break;
	case CL_INVALID_OPERATION:
		sRet = "CL_INVALID_OPERATION";
		break;
	case CL_INVALID_GL_OBJECT:
		sRet = "CL_INVALID_GL_OBJECT";
		break;
	case CL_INVALID_BUFFER_SIZE:
		sRet = "CL_INVALID_BUFFER_SIZE";
		break;
	case CL_INVALID_MIP_LEVEL:
		sRet = "CL_INVALID_MIP_LEVEL";
		break;
	case CL_INVALID_GLOBAL_WORK_SIZE:
		sRet = "CL_INVALID_GLOBAL_WORK_SIZE";
		break;
	case CL_INVALID_PROPERTY:
		sRet = "CL_INVALID_PROPERTY";
		break;
	case CL_INVALID_IMAGE_DESCRIPTOR:
		sRet = "CL_INVALID_IMAGE_DESCRIPTOR";
		break;
	case CL_INVALID_COMPILER_OPTIONS:
		sRet = "CL_INVALID_COMPILER_OPTIONS";
		break;
	case CL_INVALID_LINKER_OPTIONS:
		sRet = "CL_INVALID_LINKER_OPTIONS";
		break;
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		sRet = "CL_INVALID_DEVICE_PARTITION_COUNT";
		break;
#ifdef CL_INVALID_PIPE_SIZE
	case CL_INVALID_PIPE_SIZE:
		sRet = "CL_INVALID_PIPE_SIZE";
		break;
#endif
#ifdef CL_INVALID_DEVICE_QUEUE
	case CL_INVALID_DEVICE_QUEUE:
		sRet = "CL_INVALID_DEVICE_QUEUE";
		break;
#endif
	default:
		sRet = NULL;
	}

	return sRet;
}


const char *OclPlatformInfoToString(cl_platform_info clPlatInfo)
{
	const char *sRet = NULL;

	switch(clPlatInfo){
	case CL_PLATFORM_PROFILE:
		sRet = "Profile";
		break;
	case CL_PLATFORM_VERSION:
		sRet = "Version";
		break;
	case CL_PLATFORM_NAME:
		sRet = "Name";
		break;
	case CL_PLATFORM_VENDOR:
		sRet = "Vendor";
		break;
	case CL_PLATFORM_EXTENSIONS:
		sRet = "Extensions";
		break;
	}

	return sRet;
}


const char *OclDevieInfoToString(cl_device_info clDeviceInfo)
{
	const char *sRet = NULL;

	switch(clDeviceInfo){
	case CL_DEVICE_TYPE:
		sRet = "Device-type";
		break;
	case CL_DEVICE_NAME:
		sRet = "Name";
		break;
	case CL_DEVICE_VERSION:
		sRet = "Version";
		break;
	case CL_DEVICE_OPENCL_C_VERSION:
		sRet = "OpenCL C version";
		break;
	case CL_DRIVER_VERSION:
		sRet = "Driver-version";
		break;
	case CL_DEVICE_AVAILABLE:
		sRet = "Device available";
		break;
	case CL_DEVICE_COMPILER_AVAILABLE:
		sRet = "Compiler available";
		break;
	case CL_DEVICE_ADDRESS_BITS:
		sRet = "Address-space [bit]";
		break;
	case CL_DEVICE_MAX_COMPUTE_UNITS:
		sRet = "Max compute-units";
		break;
	case CL_DEVICE_MAX_CLOCK_FREQUENCY:
		sRet = "Max frequency [MHz]";
		break;
	case CL_DEVICE_LOCAL_MEM_SIZE:
		sRet = "Local memory size [bytes]";
		break;
	case CL_DEVICE_LOCAL_MEM_TYPE:
		sRet = "Local memory type";
		break;
	case CL_DEVICE_GLOBAL_MEM_SIZE:
		sRet = "Global memory size [bytes]";
		break;
	case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
		sRet = "Global memory cache size [bytes]";
		break;
	case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
		sRet = "Max memory allocation [bytes]";
		break;
	case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
		sRet = "Max work-item dimension";
		break;
	case CL_DEVICE_MAX_WORK_ITEM_SIZES:
		sRet = "Max work-item size";
		break;
	case CL_DEVICE_MAX_WORK_GROUP_SIZE:
		sRet = "Max work-group size";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
		sRet = "Preferred vector width: CHAR";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
		sRet = "Preferred vector width: SHORT";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
		sRet = "Preferred vector width: INT";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
		sRet = "Preferred vector width: LONG";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
		sRet = "Preferred vector width: FLOAT";
		break;
	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
		sRet = "Preferred vector width: DOUBLE";
		break;
	case CL_DEVICE_EXTENSIONS:
		sRet = "Extensions";
		break;
	case CL_DEVICE_PROFILING_TIMER_RESOLUTION:
		sRet = "Profiling timer resolution [ns]";
		break;
	}

	return sRet;
}


////////////////////////////////////////////////////////////////////////////
//	OclGetMaxWorkgroupSize
//
//  Returns:
//		MaxWorkgroupsize	(min. 1)
//		0		on error
//
size_t OclGetMaxWorkgroupSize(cl_device_id *pDeviceId)
{
	size_t zMaxWGZ;

	if(pDeviceId){
		if(!clGetDeviceInfo(*pDeviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(zMaxWGZ), &zMaxWGZ, NULL)){
			return zMaxWGZ;
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////
//	OclIsLocalMemLocal
//
//	Is dedicated local memory storage available
//
//  Returns:
//		1		true (local)
//		0		false (global)
//		-1		error
//
int OclIsLocalMemLocal(cl_device_id *pDeviceId)
{
	cl_device_local_mem_type clMemType;

	if(pDeviceId){
		if(!clGetDeviceInfo(*pDeviceId, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(clMemType), &clMemType, NULL)){
			return clMemType == CL_LOCAL ? 1 : 0;
		}
	}

	return -1;
}


////////////////////////////////////////////////////////////////////////////
//	OclGetLocalMemSize
//
//  Returns:
//		size	(min. 16384)
//		0		on error
//
cl_ulong OclGetLocalMemSize(cl_device_id *pDeviceId)
{
	cl_ulong ulLocalMemSize;

	if(pDeviceId){
		if(!clGetDeviceInfo(*pDeviceId, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(ulLocalMemSize), &ulLocalMemSize, NULL)){
			return ulLocalMemSize;
		}
	}

	return 0;
}



int FreeAllocSpace(void *pvSpace)
{
	if(pvSpace){
		free(pvSpace);
		return 0;
	}

	return 1;
}

char *GetFileStringContent(const char *sFile, size_t *zpSize)
{
	FILE *fp;
	char *sData = NULL;
	long int liSize;

	if(sFile){
		fp = fopen(sFile, "rb");
		if(fp){
			if(!fseek(fp, 0L, SEEK_END)){
				liSize = ftell(fp);
				if(liSize >= 0 && !fseek(fp, 0L, SEEK_SET)){
					sData = malloc((liSize+1)*sizeof(*sData));
					if(sData){
						if(fread(sData, sizeof(*sData), liSize, fp) == liSize){
							sData[liSize] = '\0';
							if(zpSize){
								*zpSize = liSize;
							}
						}
						else{
							free(sData);
							sData = NULL;
						}
					}
				}
			}
			fclose(fp);
		}
	}

	return sData;
}
