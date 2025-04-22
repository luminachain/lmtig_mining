


#include "crypto/randomx/randomx.h"
#include "backend/cpu/Cpu.h"
#include "crypto/rx/RxCache.h"
#include "crypto/rx/RxDataset.h"
#include "crypto/rx/RxVm.h"


randomx_vm *xmrig::RxVm::create(RxDataset *dataset, uint8_t *scratchpad, bool softAes, const Assembly &assembly, uint32_t node)
{
    int flags = 0;

    if (!softAes) {
       flags |= RANDOMX_FLAG_HARD_AES;
    }

    if (dataset->get()) {
        flags |= RANDOMX_FLAG_FULL_MEM;
    }

    if (!dataset->cache() || dataset->cache()->isJIT()) {
        flags |= RANDOMX_FLAG_JIT;
    }

    const auto asmId = assembly == Assembly::AUTO ? Cpu::info()->assembly() : assembly.id();
    if ((asmId == Assembly::RYZEN) || (asmId == Assembly::BULLDOZER)) {
        flags |= RANDOMX_FLAG_AMD;
    }

    return randomx_create_vm(static_cast<randomx_flags>(flags), !dataset->get() ? dataset->cache()->get() : nullptr, dataset->get(), scratchpad, node);
}


void xmrig::RxVm::destroy(randomx_vm* vm)
{
    if (vm) {
        randomx_destroy_vm(vm);
    }
}
