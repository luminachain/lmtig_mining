

#ifndef XMRIG_NVML_LITE_H
#define XMRIG_NVML_LITE_H


#include <cstdint>


#define NVML_SUCCESS         0
#define NVML_TEMPERATURE_GPU 0
#define NVML_CLOCK_SM        1
#define NVML_CLOCK_MEM       2


using nvmlReturn_t = uint32_t;
using nvmlDevice_t = struct nvmlDevice_st *;


struct nvmlPciInfo_t
{
    char busIdLegacy[16]{};
    unsigned int domain         = 0;
    unsigned int bus            = 0;
    unsigned int device         = 0;
    unsigned int pciDeviceId    = 0;
    unsigned int pciSubSystemId = 0;

    char busId[32]{};
};


#endif 
