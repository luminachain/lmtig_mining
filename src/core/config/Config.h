

#ifndef XMRIG_CONFIG_H
#define XMRIG_CONFIG_H


#include <cstdint>


#include "3rdparty/rapidjson/fwd.h"
#include "backend/cpu/CpuConfig.h"
#include "base/kernel/config/BaseConfig.h"
#include "base/tools/Object.h"


namespace xmrig {


class ConfigPrivate;
class CudaConfig;
class IThread;
class OclConfig;
class RxConfig;


class Config : public BaseConfig
{
public:
    XMRIG_DISABLE_COPY_MOVE(Config);

    static const char *kPauseOnBattery;
    static const char *kPauseOnActive;

#   ifdef XMRIG_FEATURE_OPENCL
    static const char *kOcl;
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    static const char *kCuda;
#   endif

#   if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
    static const char *kHealthPrintTime;
#   endif

#   ifdef XMRIG_FEATURE_DMI
    static const char *kDMI;
#   endif

    Config();
    ~Config() override;

    inline bool isPauseOnActive() const { return idleTime() > 0; }

    bool isPauseOnBattery() const;
    const CpuConfig &cpu() const;
    uint32_t idleTime() const;

#   ifdef XMRIG_FEATURE_OPENCL
    const OclConfig &cl() const;
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    const CudaConfig &cuda() const;
#   endif

#   ifdef XMRIG_ALGO_RANDOMX
    const RxConfig &rx() const;
#   endif

#   if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
    uint32_t healthPrintTime() const;
#   else
    uint32_t healthPrintTime() const        { return 0; }
#   endif

#   ifdef XMRIG_FEATURE_DMI
    bool isDMI() const;
#   else
    static constexpr inline bool isDMI()    { return false; }
#   endif

    bool isShouldSave() const;
    bool read(const IJsonReader &reader, const char *fileName) override;
    void getJSON(rapidjson::Document &doc) const override;

private:
    ConfigPrivate *d_ptr;
};


} 


#endif 
