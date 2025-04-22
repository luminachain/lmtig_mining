

#ifndef XMRIG_HUGEPAGESINFO_H
#define XMRIG_HUGEPAGESINFO_H


#include <cstdint>
#include <cstddef>


namespace xmrig {


class VirtualMemory;


class HugePagesInfo
{
public:
    HugePagesInfo() = default;
    HugePagesInfo(const VirtualMemory *memory);

    size_t allocated    = 0;
    size_t total        = 0;
    size_t size         = 0;

    inline bool isFullyAllocated() const { return allocated == total; }
    inline double percent() const        { return total == 0 ? 0.0 : static_cast<double>(allocated) / total * 100.0; }
    inline void reset()                  { allocated = 0; total = 0; size = 0; }

    inline HugePagesInfo &operator+=(const HugePagesInfo &other)
    {
        allocated += other.allocated;
        total     += other.total;
        size      += other.size;

        return *this;
    }
};


} 


#endif 
