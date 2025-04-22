

#ifndef XMRIG_CPUTHREADS_H
#define XMRIG_CPUTHREADS_H


#include <vector>


#include "backend/cpu/CpuThread.h"


namespace xmrig {


class CpuThreads
{
public:
    inline CpuThreads() = default;
    inline CpuThreads(size_t count) : m_data(count) {}

    CpuThreads(const rapidjson::Value &value);
    CpuThreads(size_t count, uint32_t intensity);

    inline bool isEmpty() const                             { return m_data.empty(); }
    inline const std::vector<CpuThread> &data() const       { return m_data; }
    inline size_t count() const                             { return m_data.size(); }
    inline void add(const CpuThread &thread)                { m_data.push_back(thread); }
    inline void add(int64_t affinity, uint32_t intensity)   { add(CpuThread(affinity, intensity)); }
    inline void reserve(size_t capacity)                    { m_data.reserve(capacity); }

    inline bool operator!=(const CpuThreads &other) const   { return !isEqual(other); }
    inline bool operator==(const CpuThreads &other) const   { return isEqual(other); }

    bool isEqual(const CpuThreads &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    enum Format {
        ArrayFormat,
        ObjectFormat
    };

    Format m_format     = ArrayFormat;
    int64_t m_affinity  = -1;
    std::vector<CpuThread> m_data;
};


} 


#endif 
