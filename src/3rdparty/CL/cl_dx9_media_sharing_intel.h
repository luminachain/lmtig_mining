


#ifndef __OPENCL_CL_DX9_MEDIA_SHARING_INTEL_H
#define __OPENCL_CL_DX9_MEDIA_SHARING_INTEL_H

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <d3d9.h>
#include <dxvahd.h>
#include <wtypes.h>
#include <d3d9types.h>

#ifdef __cplusplus
extern "C" {
#endif



#define cl_intel_dx9_media_sharing 1

typedef cl_uint cl_dx9_device_source_intel;
typedef cl_uint cl_dx9_device_set_intel;


#define CL_INVALID_DX9_DEVICE_INTEL                   -1010
#define CL_INVALID_DX9_RESOURCE_INTEL                 -1011
#define CL_DX9_RESOURCE_ALREADY_ACQUIRED_INTEL        -1012
#define CL_DX9_RESOURCE_NOT_ACQUIRED_INTEL            -1013


#define CL_D3D9_DEVICE_INTEL                          0x4022
#define CL_D3D9EX_DEVICE_INTEL                        0x4070
#define CL_DXVA_DEVICE_INTEL                          0x4071


#define CL_PREFERRED_DEVICES_FOR_DX9_INTEL            0x4024
#define CL_ALL_DEVICES_FOR_DX9_INTEL                  0x4025


#define CL_CONTEXT_D3D9_DEVICE_INTEL                  0x4026
#define CL_CONTEXT_D3D9EX_DEVICE_INTEL                0x4072
#define CL_CONTEXT_DXVA_DEVICE_INTEL                  0x4073


#define CL_MEM_DX9_RESOURCE_INTEL                     0x4027
#define CL_MEM_DX9_SHARED_HANDLE_INTEL                0x4074


#define CL_IMAGE_DX9_PLANE_INTEL                      0x4075


#define CL_COMMAND_ACQUIRE_DX9_OBJECTS_INTEL          0x402A
#define CL_COMMAND_RELEASE_DX9_OBJECTS_INTEL          0x402B


extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDsFromDX9INTEL(
    cl_platform_id              platform,
    cl_dx9_device_source_intel  dx9_device_source,
    void*                       dx9_object,
    cl_dx9_device_set_intel     dx9_device_set,
    cl_uint                     num_entries,
    cl_device_id*               devices,
    cl_uint*                    num_devices) CL_EXT_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL* clGetDeviceIDsFromDX9INTEL_fn)(
    cl_platform_id              platform,
    cl_dx9_device_source_intel  dx9_device_source,
    void*                       dx9_object,
    cl_dx9_device_set_intel     dx9_device_set,
    cl_uint                     num_entries,
    cl_device_id*               devices,
    cl_uint*                    num_devices) CL_EXT_SUFFIX__VERSION_1_1;

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromDX9MediaSurfaceINTEL(
    cl_context                  context,
    cl_mem_flags                flags,
    IDirect3DSurface9*          resource,
    HANDLE                      sharedHandle,
    UINT                        plane,
    cl_int*                     errcode_ret) CL_EXT_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromDX9MediaSurfaceINTEL_fn)(
    cl_context                  context,
    cl_mem_flags                flags,
    IDirect3DSurface9*          resource,
    HANDLE                      sharedHandle,
    UINT                        plane,
    cl_int*                     errcode_ret) CL_EXT_SUFFIX__VERSION_1_1;

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireDX9ObjectsINTEL(
    cl_command_queue            command_queue,
    cl_uint                     num_objects,
    const cl_mem*               mem_objects,
    cl_uint                     num_events_in_wait_list,
    const cl_event*             event_wait_list,
    cl_event*                   event) CL_EXT_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueAcquireDX9ObjectsINTEL_fn)(
    cl_command_queue            command_queue,
    cl_uint                     num_objects,
    const cl_mem*               mem_objects,
    cl_uint                     num_events_in_wait_list,
    const cl_event*             event_wait_list,
    cl_event*                   event) CL_EXT_SUFFIX__VERSION_1_1;

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseDX9ObjectsINTEL(
    cl_command_queue            command_queue,
    cl_uint                     num_objects,
    cl_mem*                     mem_objects,
    cl_uint                     num_events_in_wait_list,
    const cl_event*             event_wait_list,
    cl_event*                   event) CL_EXT_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueReleaseDX9ObjectsINTEL_fn)(
    cl_command_queue            command_queue,
    cl_uint                     num_objects,
    cl_mem*                     mem_objects,
    cl_uint                     num_events_in_wait_list,
    const cl_event*             event_wait_list,
    cl_event*                   event) CL_EXT_SUFFIX__VERSION_1_1;

#ifdef __cplusplus
}
#endif

#endif  

