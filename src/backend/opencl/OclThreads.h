

#ifndef XMRIG_OCLTHREADS_H
#define XMRIG_OCLTHREADS_H


#include <vector>


#include "backend/opencl/OclThread.h"
#include "backend/opencl/wrappers/OclDevice.h"


namespace xmrig {


class OclThreads
{
public:
    OclThreads() = default;
    OclThreads(const rapidjson::Value &value);
    OclThreads(const std::vector<OclDevice> &devices, const Algorithm &algorithm);

    inline bool isEmpty() const                             { return m_data.empty(); }
    inline const std::vector<OclThread> &data() const       { return m_data; }
    inline size_t count() const                             { return m_data.size(); }
    inline void add(OclThread &&thread)                     { m_data.push_back(thread); }
    inline void reserve(size_t capacity)                    { m_data.reserve(capacity); }

    inline bool operator!=(const OclThreads &other) const   { return !isEqual(other); }
    inline bool operator==(const OclThreads &other) const   { return isEqual(other); }

    bool isEqual(const OclThreads &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    std::vector<OclThread> m_data;
};


} 


#endif 
