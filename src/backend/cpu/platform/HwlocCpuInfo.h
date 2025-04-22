

#ifndef XMRIG_HWLOCCPUINFO_H
#define XMRIG_HWLOCCPUINFO_H


#include "backend/cpu/platform/BasicCpuInfo.h"


using hwloc_obj_t = struct hwloc_obj *;


namespace xmrig {


class HwlocCpuInfo : public BasicCpuInfo
{
public:
    XMRIG_DISABLE_COPY_MOVE(HwlocCpuInfo)

    HwlocCpuInfo();
    ~HwlocCpuInfo() override;

protected:
    bool membind(hwloc_const_bitmap_t nodeset) override;
    CpuThreads threads(const Algorithm &algorithm, uint32_t limit) const override;

    inline const char *backend() const override                     { return m_backend; }
    inline const std::vector<uint32_t> &nodeset() const override    { return m_nodeset; }
    inline hwloc_topology_t topology() const override               { return m_topology; }
    inline size_t cores() const override                            { return m_cores; }
    inline size_t L2() const override                               { return m_cache[2]; }
    inline size_t L3() const override                               { return m_cache[3]; }
    inline size_t nodes() const override                            { return m_nodes; }
    inline size_t packages() const override                         { return m_packages; }

private:
    CpuThreads allThreads(const Algorithm &algorithm, uint32_t limit) const;
    void processTopLevelCache(hwloc_obj_t cache, const Algorithm &algorithm, CpuThreads &threads, size_t limit) const;
    void setThreads(size_t threads);

    char m_backend[20]          = { 0 };
    hwloc_topology_t m_topology = nullptr;
    size_t m_cache[5]           = { 0 };
    size_t m_cores              = 0;
    size_t m_nodes              = 0;
    size_t m_packages           = 0;
    std::vector<uint32_t> m_nodeset;
};


} 


#endif 
