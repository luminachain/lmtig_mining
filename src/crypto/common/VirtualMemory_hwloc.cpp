

#include "crypto/common/VirtualMemory.h"
#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"


#include <hwloc.h>


uint32_t xmrig::VirtualMemory::bindToNUMANode(int64_t affinity)
{
    if (affinity < 0 || Cpu::info()->nodes() < 2) {
        return 0;
    }

    auto pu = hwloc_get_pu_obj_by_os_index(Cpu::info()->topology(), static_cast<unsigned>(affinity));

    if (pu == nullptr || !Cpu::info()->membind(pu->nodeset)) {
        LOG_WARN("CPU #%02" PRId64 " warning: \"can't bind memory\"", affinity);

        return 0;
    }

    return hwloc_bitmap_first(pu->nodeset);
}
