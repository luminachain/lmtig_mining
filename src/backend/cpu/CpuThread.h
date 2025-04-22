

#ifndef XMRIG_CPUTHREAD_H
#define XMRIG_CPUTHREAD_H


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class CpuThread
{
public:
    inline constexpr CpuThread() = default;
    inline constexpr CpuThread(int64_t affinity, uint32_t intensity) : m_affinity(affinity), m_intensity(intensity) {}

    CpuThread(const rapidjson::Value &value);

    inline bool isEqual(const CpuThread &other) const       { return other.m_affinity == m_affinity && other.m_intensity == m_intensity; }
    inline bool isValid() const                             { return m_intensity <= 8; }
    inline int64_t affinity() const                         { return m_affinity; }
    inline uint32_t intensity() const                       { return m_intensity == 0 ? 1 : m_intensity; }

    inline bool operator!=(const CpuThread &other) const    { return !isEqual(other); }
    inline bool operator==(const CpuThread &other) const    { return isEqual(other); }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    int64_t m_affinity   = -1;
    uint32_t m_intensity = 0;
};


} 


#endif 
