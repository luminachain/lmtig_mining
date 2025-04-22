

#ifndef XMRIG_CUDATHREADS_H
#define XMRIG_CUDATHREADS_H


#include <vector>


#include "backend/cuda/CudaThread.h"
#include "backend/cuda/wrappers/CudaDevice.h"


namespace xmrig {


class CudaThreads
{
public:
    CudaThreads() = default;
    CudaThreads(const rapidjson::Value &value);
    CudaThreads(const std::vector<CudaDevice> &devices, const Algorithm &algorithm);

    inline bool isEmpty() const                              { return m_data.empty(); }
    inline const std::vector<CudaThread> &data() const       { return m_data; }
    inline size_t count() const                              { return m_data.size(); }
    inline void add(const CudaThread &thread)                { m_data.push_back(thread); }
    inline void reserve(size_t capacity)                     { m_data.reserve(capacity); }

    inline bool operator!=(const CudaThreads &other) const   { return !isEqual(other); }
    inline bool operator==(const CudaThreads &other) const   { return isEqual(other); }

    bool isEqual(const CudaThreads &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    std::vector<CudaThread> m_data;
};


} 


#endif 
