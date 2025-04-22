

#ifndef XMRIG_TAGS_H
#define XMRIG_TAGS_H


#include <cstddef>
#include <cstdint>


namespace xmrig {


class Tags
{
public:
    static const char *config();
    static const char *network();
    static const char *origin();
    static const char *signal();

#   ifdef XMRIG_MINER_PROJECT
    static const char *cpu();
    static const char *miner();
#   ifdef XMRIG_ALGO_RANDOMX
    static const char *randomx();
#   endif
#   ifdef XMRIG_FEATURE_BENCHMARK
    static const char *bench();
#   endif
#   endif

#   ifdef XMRIG_PROXY_PROJECT
    static const char *proxy();
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    static const char *nvidia();
#   endif

#   ifdef XMRIG_FEATURE_OPENCL
    static const char *opencl();
#   endif

#   ifdef XMRIG_FEATURE_PROFILING
    static const char* profiler();
#   endif
};


} 


#endif 
