

#include "hw/msr/Msr.h"
#include "base/io/log/Log.h"


namespace xmrig {


static const char *kTag = YELLOW_BG_BOLD(WHITE_BOLD_S " msr     ");
static std::weak_ptr<Msr> instance;


} 



const char *xmrig::Msr::tag()
{
    return kTag;
}



std::shared_ptr<xmrig::Msr> xmrig::Msr::get()
{
    auto msr = instance.lock();
    if (!msr) {
        msr      = std::make_shared<Msr>();
        instance = msr;
    }

    if (msr->isAvailable()) {
        return msr;
    }

    return {};
}


bool xmrig::Msr::write(uint32_t reg, uint64_t value, int32_t cpu, uint64_t mask, bool verbose)
{
    if (mask != MsrItem::kNoMask) {
        uint64_t old_value = 0;
        if (rdmsr(reg, cpu, old_value)) {
            value = MsrItem::maskedValue(old_value, value, mask);
        }
    }

    const bool result = wrmsr(reg, value, cpu);
    if (!result && verbose) {
        LOG_WARN("%s " YELLOW_BOLD("cannot set MSR 0x%08" PRIx32 " to 0x%016" PRIx64), tag(), reg, value);
    }

    return result;
}


xmrig::MsrItem xmrig::Msr::read(uint32_t reg, int32_t cpu, bool verbose) const
{
    uint64_t value = 0;
    if (rdmsr(reg, cpu, value)) {
        return { reg, value };
    }

    if (verbose) {
        LOG_WARN("%s " YELLOW_BOLD("cannot read MSR 0x%08" PRIx32), tag(), reg);
    }

    return {};
}
