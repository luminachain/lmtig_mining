

#ifndef XMRIG_OCLKAWPOW_H
#define XMRIG_OCLKAWPOW_H


#include <cstddef>
#include <cstdint>


using cl_kernel = struct _cl_kernel *;


namespace xmrig {


class IOclRunner;


class OclKawPow
{
public:
    static cl_kernel get(const IOclRunner &runner, uint64_t height, uint32_t worksize);
    static void clear();
};


} 


#endif 
