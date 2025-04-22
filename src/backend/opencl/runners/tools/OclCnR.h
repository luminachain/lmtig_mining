

#ifndef XMRIG_OCLCNR_H
#define XMRIG_OCLCNR_H


#include <cstddef>
#include <cstdint>


using cl_program = struct _cl_program *;


namespace xmrig {


class Algorithm;
class IOclRunner;


class OclCnR
{
public:
    constexpr static size_t kHeightChunkSize = 10;

    static cl_program get(const IOclRunner &runner, uint64_t height);
    static void clear();
};


} 


#endif 
