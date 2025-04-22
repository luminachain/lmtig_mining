

#ifndef __OPENCL_CL_GL_EXT_H
#define __OPENCL_CL_GL_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <CL/cl_gl.h>


#define CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR     0x200D

extern CL_API_ENTRY cl_event CL_API_CALL
clCreateEventFromGLsyncKHR(cl_context context,
                           cl_GLsync  cl_GLsync,
                           cl_int *   errcode_ret) CL_EXT_SUFFIX__VERSION_1_1;

#ifdef __cplusplus
}
#endif

#endif	
