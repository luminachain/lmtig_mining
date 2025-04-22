

#ifndef XMRIG_ADLHEALTH_H
#define XMRIG_ADLHEALTH_H


#include <cstdint>
#include <vector>


struct AdlHealth
{
    uint32_t clock          = 0;
    uint32_t memClock       = 0;
    uint32_t power          = 0;
    uint32_t rpm            = 0;
    uint32_t temperature    = 0;
};


#endif 
