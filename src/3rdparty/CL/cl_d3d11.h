



#ifndef __OPENCL_CL_D3D11_H
#define __OPENCL_CL_D3D11_H

#include <d3d11.h>
#include <CL/cl.h>
#include <CL/cl_platform.h>

#ifdef __cplusplus
extern "C" {
#endif


#define cl_khr_d3d11_sharing 1

typedef cl_uint cl_d3d11_device_source_khr;
typedef cl_uint cl_d3d11_device_set_khr;




#define CL_INVALID_D3D11_DEVICE_KHR                  -1006
#define CL_INVALID_D3D11_RESOURCE_KHR                -1007
#define CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR       -1008
#define CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR           -1009


#define CL_D3D11_DEVICE_KHR                          0x4019
#define CL_D3D11_DXGI_ADAPTER_KHR                    0x401A


#define CL_PREFERRED_DEVICES_FOR_D3D11_KHR           0x401B
#define CL_ALL_DEVICES_FOR_D3D11_KHR                 0x401C


#define CL_CONTEXT_D3D11_DEVICE_KHR                  0x401D
#define CL_CONTEXT_D3D11_PREFER_SHARED_RESOURCES_KHR 0x402D


#define CL_MEM_D3D11_RESOURCE_KHR                    0x401E


#define CL_IMAGE_D3D11_SUBRESOURCE_KHR               0x401F


#define CL_COMMAND_ACQUIRE_D3D11_OBJECTS_KHR         0x4020
#define CL_COMMAND_RELEASE_D3D11_OBJECTS_KHR         0x4021



typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetDeviceIDsFromD3D11KHR_fn)(
    cl_platform_id             platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void *                     d3d_object,
    cl_d3d11_device_set_khr    d3d_device_set,
    cl_uint                    num_entries,
    cl_device_id *             devices,
    cl_uint *                  num_devices) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11BufferKHR_fn)(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D11Buffer * resource,
    cl_int *       errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture2DKHR_fn)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture3DKHR_fn)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueAcquireD3D11ObjectsKHR_fn)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueReleaseD3D11ObjectsKHR_fn)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

#ifdef __cplusplus
}
#endif

#endif  

