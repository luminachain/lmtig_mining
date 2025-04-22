


#include "crypto/common/VirtualMemory.h"
#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"
#include "crypto/common/MemoryPool.h"
#include "crypto/common/portable/mm_malloc.h"


#ifdef XMRIG_FEATURE_HWLOC
#   include "crypto/common/NUMAMemoryPool.h"
#endif


#include <cinttypes>
#include <mutex>


namespace xmrig {


size_t VirtualMemory::m_hugePageSize    = VirtualMemory::kDefaultHugePageSize;
static IMemoryPool *pool                = nullptr;
static std::mutex mutex;


} 


xmrig::VirtualMemory::VirtualMemory(size_t size, bool hugePages, bool oneGbPages, bool usePool, uint32_t node, size_t alignSize) :
    m_size(alignToHugePageSize(size)),
    m_node(node),
    m_capacity(m_size)
{
    if (usePool) {
        std::lock_guard<std::mutex> lock(mutex);
        if (hugePages && !pool->isHugePages(node) && allocateLargePagesMemory()) {
            return;
        }

        m_scratchpad = pool->get(m_size, node);
        if (m_scratchpad) {
            m_flags.set(FLAG_HUGEPAGES, pool->isHugePages(node));
            m_flags.set(FLAG_EXTERNAL,  true);

            return;
        }
    }

    if (oneGbPages && allocateOneGbPagesMemory()) {
        m_capacity = align(size, 1ULL << 30);
        return;
    }

    if (hugePages && allocateLargePagesMemory()) {
        return;
    }

    m_scratchpad = static_cast<uint8_t*>(_mm_malloc(m_size, alignSize));
}


xmrig::VirtualMemory::~VirtualMemory()
{
    if (!m_scratchpad) {
        return;
    }

    if (m_flags.test(FLAG_EXTERNAL)) {
        std::lock_guard<std::mutex> lock(mutex);
        pool->release(m_node);
    }
    else if (isHugePages() || isOneGbPages()) {
        freeLargePagesMemory();
    }
    else {
        _mm_free(m_scratchpad);
    }
}


xmrig::HugePagesInfo xmrig::VirtualMemory::hugePages() const
{
    return { this };
}


#ifndef XMRIG_FEATURE_HWLOC
uint32_t xmrig::VirtualMemory::bindToNUMANode(int64_t)
{
    return 0;
}
#endif


void xmrig::VirtualMemory::destroy()
{
    delete pool;
}


void xmrig::VirtualMemory::init(size_t poolSize, size_t hugePageSize)
{
    if (!pool) {
        osInit(hugePageSize);
    }

#   ifdef XMRIG_FEATURE_HWLOC
    if (Cpu::info()->nodes() > 1) {
        pool = new NUMAMemoryPool(align(poolSize, Cpu::info()->nodes()), hugePageSize > 0);
    } else
#   endif
    {
        pool = new MemoryPool(poolSize, hugePageSize > 0);
    }
}
