

#ifndef XMRIG_RXMSR_H
#define XMRIG_RXMSR_H


#include <vector>


namespace xmrig
{


class CpuThread;
class RxConfig;


class RxMsr
{
public:
    static inline bool isEnabled()      { return m_enabled; }
    static inline bool isInitialized()  { return m_initialized; }

    static bool init(const RxConfig &config, const std::vector<CpuThread> &threads);
    static void destroy();

private:
    static bool m_cacheQoS;
    static bool m_enabled;
    static bool m_initialized;
};


} 


#endif 
