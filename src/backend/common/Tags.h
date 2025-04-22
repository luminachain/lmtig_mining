

#ifndef XMRIG_BACKEND_TAGS_H
#define XMRIG_BACKEND_TAGS_H


#include <cstdint>


namespace xmrig {


const char *backend_tag(uint32_t backend);
const char *cpu_tag();


#ifdef XMRIG_FEATURE_OPENCL
const char *ocl_tag();
#endif


#ifdef XMRIG_FEATURE_CUDA
const char *cuda_tag();
#endif


} 


#endif 
