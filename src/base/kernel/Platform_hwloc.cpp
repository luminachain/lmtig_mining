

#include "base/kernel/Platform.h"
#include "backend/cpu/Cpu.h"


#include <hwloc.h>
#include <thread>


#ifndef XMRIG_OS_APPLE
bool xmrig::Platform::setThreadAffinity(uint64_t cpu_id)
{
    auto topology = Cpu::info()->topology();
    auto pu       = hwloc_get_pu_obj_by_os_index(topology, static_cast<unsigned>(cpu_id));

    if (pu == nullptr) {
        return false;
    }

    if (hwloc_set_cpubind(topology, pu->cpuset, HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT) >= 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return true;
    }

    const bool result = (hwloc_set_cpubind(topology, pu->cpuset, HWLOC_CPUBIND_THREAD) >= 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return result;
}
#endif
