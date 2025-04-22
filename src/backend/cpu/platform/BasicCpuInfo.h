

#ifndef XMRIG_BASICCPUINFO_H
#define XMRIG_BASICCPUINFO_H


#include "backend/cpu/interfaces/ICpuInfo.h"


#include <bitset>


namespace xmrig {


class BasicCpuInfo : public ICpuInfo
{
public:
    BasicCpuInfo();

protected:
    const char *backend() const override;
    CpuThreads threads(const Algorithm &algorithm, uint32_t limit) const override;
    rapidjson::Value toJSON(rapidjson::Document &doc) const override;

    inline Arch arch() const override                           { return m_arch; }
    inline Assembly::Id assembly() const override               { return m_assembly; }
    inline bool has(Flag flag) const override                   { return m_flags.test(flag); }
    inline bool hasAES() const override                         { return has(FLAG_AES); }
    inline bool hasVAES() const override                        { return has(FLAG_VAES); }
    inline bool hasAVX() const override                         { return has(FLAG_AVX); }
    inline bool hasAVX2() const override                        { return has(FLAG_AVX2); }
    inline bool hasBMI2() const override                        { return has(FLAG_BMI2); }
    inline bool hasCatL3() const override                       { return has(FLAG_CAT_L3); }
    inline bool hasOneGbPages() const override                  { return has(FLAG_PDPE1GB); }
    inline bool hasXOP() const override                         { return has(FLAG_XOP); }
    inline bool isVM() const override                           { return has(FLAG_VM); }
    inline bool jccErratum() const override                     { return m_jccErratum; }
    inline const char *brand() const override                   { return m_brand; }
    inline const std::vector<int32_t> &units() const override   { return m_units; }
    inline MsrMod msrMod() const override                       { return m_msrMod; }
    inline size_t cores() const override                        { return 0; }
    inline size_t L2() const override                           { return 0; }
    inline size_t L3() const override                           { return 0; }
    inline size_t nodes() const override                        { return 0; }
    inline size_t packages() const override                     { return 1; }
    inline size_t threads() const override                      { return m_threads; }
    inline Vendor vendor() const override                       { return m_vendor; }
    inline uint32_t model() const override
    {
#   ifndef XMRIG_ARM
        return m_model;
#   else
        return 0;
#   endif
    }

    Arch m_arch             = ARCH_UNKNOWN;
    bool m_jccErratum       = false;
    char m_brand[64 + 6]{};
    size_t m_threads        = 0;
    std::vector<int32_t> m_units;
    Vendor m_vendor         = VENDOR_UNKNOWN;

private:
#   ifndef XMRIG_ARM
    uint32_t m_procInfo     = 0;
    uint32_t m_family       = 0;
    uint32_t m_model        = 0;
    uint32_t m_stepping     = 0;
#   endif

    Assembly m_assembly     = Assembly::NONE;
    MsrMod m_msrMod         = MSR_MOD_NONE;
    std::bitset<FLAG_MAX> m_flags;
};


} 


#endif 
