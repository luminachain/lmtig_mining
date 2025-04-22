

#ifndef XMRIG_NVMLHEALTH_H
#define XMRIG_NVMLHEALTH_H


#include <cstdint>
#include <vector>


struct NvmlHealth
{
    std::vector<uint32_t> fanSpeed;
    uint32_t clock          = 0;
    uint32_t memClock       = 0;
    uint32_t power          = 0;
    uint32_t temperature    = 0;
};


#endif 
