


#include "backend/common/Tags.h"
#include "backend/opencl/wrappers/OclError.h"
#include "backend/opencl/wrappers/OclKernel.h"
#include "backend/opencl/wrappers/OclLib.h"
#include "base/io/log/Log.h"


#include <stdexcept>


xmrig::OclKernel::OclKernel(cl_program program, const char *name) :
    m_name(name)
{
    m_kernel = OclLib::createKernel(program, name);
}


xmrig::OclKernel::~OclKernel()
{
    OclLib::release(m_kernel);
}


void xmrig::OclKernel::enqueueNDRange(cl_command_queue queue, uint32_t work_dim, const size_t *global_work_offset, const size_t *global_work_size, const size_t *local_work_size)
{
    const cl_int ret = OclLib::enqueueNDRangeKernel(queue, m_kernel, work_dim, global_work_offset, global_work_size, local_work_size, 0, nullptr, nullptr);
    if (ret != CL_SUCCESS) {
        LOG_ERR("%s" RED(" error ") RED_BOLD("%s") RED(" when calling ") RED_BOLD("clEnqueueNDRangeKernel") RED(" for kernel ") RED_BOLD("%s"),
                ocl_tag(), OclError::toString(ret), name().data());

        throw std::runtime_error(OclError::toString(ret));
    }
}


void xmrig::OclKernel::setArg(uint32_t index, size_t size, const void *value)
{
    const cl_int ret = OclLib::setKernelArg(m_kernel, index, size, value);
    if (ret != CL_SUCCESS) {
        LOG_ERR("%s" RED(" error ") RED_BOLD("%s") RED(" when calling ") RED_BOLD("clSetKernelArg") RED(" for kernel ") RED_BOLD("%s")
                RED(" argument ") RED_BOLD("%u") RED(" size ") RED_BOLD("%zu"),
                ocl_tag(), OclError::toString(ret), name().data(), index, size);

        throw std::runtime_error(OclError::toString(ret));
    }
}
