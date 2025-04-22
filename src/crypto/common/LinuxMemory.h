

#ifndef XMRIG_LINUXMEMORY_H
#define XMRIG_LINUXMEMORY_H


#include <cstdint>
#include <cstddef>


namespace xmrig {


class LinuxMemory
{
public:
    static bool reserve(size_t size, uint32_t node, size_t hugePageSize);

    static bool write(const char *path, uint64_t value);
    static int64_t read(const char *path);
};


} 


#endif 
