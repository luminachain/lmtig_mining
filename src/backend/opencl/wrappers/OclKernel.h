

#ifndef XMRIG_OCLKERNEL_H
#define XMRIG_OCLKERNEL_H


#include "base/tools/Object.h"
#include "base/tools/String.h"


using cl_command_queue  = struct _cl_command_queue *;
using cl_kernel         = struct _cl_kernel *;
using cl_mem            = struct _cl_mem *;
using cl_program        = struct _cl_program *;


namespace xmrig {


class OclKernel
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclKernel)

    OclKernel(cl_program program, const char *name);
    virtual ~OclKernel();

    inline bool isValid() const         { return m_kernel != nullptr; }
    inline cl_kernel kernel() const     { return m_kernel; }
    inline const String &name() const   { return m_name; }

    void enqueueNDRange(cl_command_queue queue, uint32_t work_dim, const size_t *global_work_offset, const size_t *global_work_size, const size_t *local_work_size);
    void setArg(uint32_t index, size_t size, const void *value);

private:
    cl_kernel m_kernel = nullptr;
    const String m_name;
};


} 


#endif 
