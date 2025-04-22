


#include "crypto/common/NUMAMemoryPool.h"
#include "crypto/common/VirtualMemory.h"
#include "backend/cpu/Cpu.h"
#include "crypto/common/MemoryPool.h"


#include <algorithm>


xmrig::NUMAMemoryPool::NUMAMemoryPool(size_t size, bool hugePages) :
    m_hugePages(hugePages),
    m_nodeSize(std::max<size_t>(size / Cpu::info()->nodes(), 1)),
    m_size(size)
{
}


xmrig::NUMAMemoryPool::~NUMAMemoryPool()
{
    for (auto kv : m_map) {
        delete kv.second;
    }
}


bool xmrig::NUMAMemoryPool::isHugePages(uint32_t node) const
{
    if (!m_size) {
        return false;
    }

    return getOrCreate(node)->isHugePages(node);
}


uint8_t *xmrig::NUMAMemoryPool::get(size_t size, uint32_t node)
{
    if (!m_size) {
        return nullptr;
    }

    return getOrCreate(node)->get(size, node);
}


void xmrig::NUMAMemoryPool::release(uint32_t node)
{
    const auto pool = get(node);
    if (pool) {
        pool->release(node);
    }
}


xmrig::IMemoryPool *xmrig::NUMAMemoryPool::get(uint32_t node) const
{
    return m_map.count(node) ? m_map.at(node) : nullptr;
}


xmrig::IMemoryPool *xmrig::NUMAMemoryPool::getOrCreate(uint32_t node) const
{
    auto pool = get(node);
    if (!pool) {
        pool = new MemoryPool(m_nodeSize, m_hugePages, node);
        m_map.insert({ node, pool });
    }

    return pool;
}
