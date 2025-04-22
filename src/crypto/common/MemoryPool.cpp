


#include "crypto/common/MemoryPool.h"
#include "crypto/common/VirtualMemory.h"


#include <cassert>


namespace xmrig {


constexpr size_t pageSize = 2 * 1024 * 1024;


} 


xmrig::MemoryPool::MemoryPool(size_t size, bool hugePages, uint32_t node)
{
    if (!size) {
        return;
    }

    constexpr size_t alignment = 1 << 24;

    m_memory = new VirtualMemory(size * pageSize + alignment, hugePages, false, false, node);

    m_alignOffset = (alignment - (((size_t)m_memory->scratchpad()) % alignment)) % alignment;
}


xmrig::MemoryPool::~MemoryPool()
{
    delete m_memory;
}


bool xmrig::MemoryPool::isHugePages(uint32_t) const
{
    return m_memory && m_memory->isHugePages();
}


uint8_t *xmrig::MemoryPool::get(size_t size, uint32_t)
{
    assert(!(size % pageSize));

    if (!m_memory || (m_memory->size() - m_offset - m_alignOffset) < size) {
        return nullptr;
    }

    uint8_t *out = m_memory->scratchpad() + m_alignOffset + m_offset;

    m_offset += size;
    ++m_refs;

    return out;
}


void xmrig::MemoryPool::release(uint32_t)
{
    assert(m_refs > 0);

    if (m_refs > 0) {
        --m_refs;
    }

    if (m_refs == 0) {
        m_offset = 0;
    }
}
