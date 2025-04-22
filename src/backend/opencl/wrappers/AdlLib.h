

#ifndef XMRIG_ADLLIB_H
#define XMRIG_ADLLIB_H


#include "backend/opencl/wrappers/AdlHealth.h"


namespace xmrig {


class OclDevice;


class AdlLib
{
public:
    static bool init();
    static const char *lastError() noexcept;
    static void close();

    static AdlHealth health(const OclDevice &device);

    static inline bool isInitialized() noexcept         { return m_initialized; }
    static inline bool isReady() noexcept               { return m_ready; }

private:
    static bool dlopen();
    static bool load();

    static bool m_initialized;
    static bool m_ready;
};


} 


#endif 
