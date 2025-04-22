

#ifndef XMRIG_CUDATHREAD_H
#define XMRIG_CUDATHREAD_H


using nvid_ctx = struct nvid_ctx;


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class CudaThread
{
public:
    CudaThread() = delete;
    CudaThread(const rapidjson::Value &value);
    CudaThread(uint32_t index, nvid_ctx *ctx);

    inline bool isValid() const                              { return m_blocks > 0 && m_threads > 0; }
    inline int32_t bfactor() const                           { return static_cast<int32_t>(m_bfactor); }
    inline int32_t blocks() const                            { return m_blocks; }
    inline int32_t bsleep() const                            { return static_cast<int32_t>(m_bsleep); }
    inline int32_t datasetHost() const                       { return m_datasetHost; }
    inline int32_t threads() const                           { return m_threads; }
    inline int64_t affinity() const                          { return m_affinity; }
    inline uint32_t index() const                            { return m_index; }

    inline bool operator!=(const CudaThread &other) const    { return !isEqual(other); }
    inline bool operator==(const CudaThread &other) const    { return isEqual(other); }

    bool isEqual(const CudaThread &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    int32_t m_blocks        = 0;
    int32_t m_datasetHost   = -1;
    int32_t m_threads       = 0;
    int64_t m_affinity      = -1;
    uint32_t m_index        = 0;

#   ifdef _WIN32
    uint32_t m_bfactor      = 6;
    uint32_t m_bsleep       = 25;
#   else
    uint32_t m_bfactor      = 0;
    uint32_t m_bsleep       = 0;
#   endif
};


} 


#endif 
