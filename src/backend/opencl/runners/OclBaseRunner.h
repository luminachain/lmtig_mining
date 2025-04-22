

#ifndef XMRIG_OCLBASERUNNER_H
#define XMRIG_OCLBASERUNNER_H


#include <string>


#include "3rdparty/cl.h"
#include "backend/opencl/interfaces/IOclRunner.h"
#include "base/crypto/Algorithm.h"


namespace xmrig {


class OclLaunchData;


class OclBaseRunner : public IOclRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclBaseRunner)

    OclBaseRunner(size_t id, const OclLaunchData &data);
    ~OclBaseRunner() override;

protected:
    inline cl_context ctx() const override                { return m_ctx; }
    inline const Algorithm &algorithm() const override    { return m_algorithm; }
    inline const char *buildOptions() const override      { return m_options.c_str(); }
    inline const char *deviceKey() const override         { return m_deviceKey.c_str(); }
    inline const char *source() const override            { return m_source; }
    inline const OclLaunchData &data() const override     { return m_data; }
    inline size_t intensity() const override              { return m_intensity; }
    inline size_t threadId() const override               { return m_threadId; }
    inline uint32_t roundSize() const override            { return m_intensity; }
    inline uint32_t processedHashes() const override      { return m_intensity; }
    inline void jobEarlyNotification(const Job&) override {}

    size_t bufferSize() const override;
    uint32_t deviceIndex() const override;
    void build() override;
    void init() override;

protected:
    cl_mem createSubBuffer(cl_mem_flags flags, size_t size);
    size_t align(size_t size) const;
    void enqueueReadBuffer(cl_mem buffer, cl_bool blocking_read, size_t offset, size_t size, void *ptr);
    void enqueueWriteBuffer(cl_mem buffer, cl_bool blocking_write, size_t offset, size_t size, const void *ptr);
    void finalize(uint32_t *hashOutput);

    cl_command_queue m_queue    = nullptr;
    cl_context m_ctx;
    cl_mem m_buffer             = nullptr;
    cl_mem m_input              = nullptr;
    cl_mem m_output             = nullptr;
    cl_program m_program        = nullptr;
    const Algorithm m_algorithm;
    const char *m_source;
    const OclLaunchData &m_data;
    const size_t m_align;
    const size_t m_threadId;
    const uint32_t m_intensity;
    size_t m_offset             = 0;
    std::string m_deviceKey;
    std::string m_options;
};


} 


#endif 
