

#ifndef XMRIG_NVMLLIB_H
#define XMRIG_NVMLLIB_H


#include "backend/cuda/wrappers/CudaDevice.h"
#include "backend/cuda/wrappers/NvmlHealth.h"


namespace xmrig {


class NvmlLib
{
public:
    static bool init(const char *fileName = nullptr);
    static const char *lastError() noexcept;
    static void close();

    static bool assign(std::vector<CudaDevice> &devices);
    static NvmlHealth health(nvmlDevice_t device);

    static inline bool isInitialized() noexcept         { return m_initialized; }
    static inline bool isReady() noexcept               { return m_ready; }
    static inline const char *driverVersion() noexcept  { return m_driverVersion; }
    static inline const char *version() noexcept        { return m_nvmlVersion; }

private:
    static bool dlopen();
    static bool load();

    static bool m_initialized;
    static bool m_ready;
    static char m_driverVersion[80];
    static char m_nvmlVersion[80];
    static String m_loader;
};


} 


#endif 
