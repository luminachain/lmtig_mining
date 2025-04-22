


#include "crypto/rx/RxCache.h"
#include "crypto/common/VirtualMemory.h"
#include "crypto/randomx/randomx.h"


static_assert(RANDOMX_FLAG_JIT == 8, "RANDOMX_FLAG_JIT flag mismatch");


xmrig::RxCache::RxCache(bool hugePages, uint32_t nodeId)
{
    m_memory = new VirtualMemory(maxSize(), hugePages, false, false, nodeId);

    create(m_memory->raw());
}


xmrig::RxCache::RxCache(uint8_t *memory)
{
    create(memory);
}


xmrig::RxCache::~RxCache()
{
    randomx_release_cache(m_cache);

    delete m_memory;
}


bool xmrig::RxCache::init(const Buffer &seed)
{
    if (m_seed == seed) {
        return false;
    }

    m_seed = seed;

    if (m_cache) {
        randomx_init_cache(m_cache, m_seed.data(), m_seed.size());

        return true;
    }

    return false;
}


xmrig::HugePagesInfo xmrig::RxCache::hugePages() const
{
    return m_memory ? m_memory->hugePages() : HugePagesInfo();
}


void xmrig::RxCache::create(uint8_t *memory)
{
    if (!memory) {
        return;
    }

    m_cache = randomx_create_cache(RANDOMX_FLAG_JIT, memory);

    if (!m_cache) {
        m_jit   = false;
        m_cache = randomx_create_cache(RANDOMX_FLAG_DEFAULT, memory);
    }
}
