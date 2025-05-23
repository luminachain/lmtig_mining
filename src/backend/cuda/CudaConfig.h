

#ifndef XMRIG_CUDACONFIG_H
#define XMRIG_CUDACONFIG_H


#include "backend/cuda/CudaLaunchData.h"
#include "backend/common/Threads.h"
#include "backend/cuda/CudaThreads.h"


namespace xmrig {


class CudaConfig
{
public:
    CudaConfig() = default;

    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    std::vector<CudaLaunchData> get(const Miner *miner, const Algorithm &algorithm, const std::vector<CudaDevice> &devices) const;
    void read(const rapidjson::Value &value);

    inline bool isEnabled() const                               { return m_enabled; }
    inline bool isShouldSave() const                            { return m_shouldSave; }
    inline const std::vector<uint32_t> &devicesHint() const     { return m_devicesHint; }
    inline const String &loader() const                         { return m_loader; }
    inline const Threads<CudaThreads> &threads() const          { return m_threads; }
    inline int32_t bfactor() const                              { return m_bfactor; }
    inline int32_t bsleep() const                               { return m_bsleep; }

#   ifdef XMRIG_FEATURE_NVML
    inline bool isNvmlEnabled() const                           { return m_nvml; }
    inline const String &nvmlLoader() const                     { return m_nvmlLoader; }
#   endif

private:
    void generate();
    void setDevicesHint(const char *devicesHint);

    bool m_enabled          = false;
    bool m_shouldSave       = false;
    std::vector<uint32_t> m_devicesHint;
    String m_loader;
    Threads<CudaThreads> m_threads;

#   ifdef _WIN32
    int32_t m_bfactor      = 6;
    int32_t m_bsleep       = 25;
#   else
    int32_t m_bfactor      = 0;
    int32_t m_bsleep       = 0;
#   endif

#   ifdef XMRIG_FEATURE_NVML
    bool m_nvml            = true;
    String m_nvmlLoader;
#   endif
};


} 


#endif 
