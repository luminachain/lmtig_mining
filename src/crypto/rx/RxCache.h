

#ifndef XMRIG_RX_CACHE_H
#define XMRIG_RX_CACHE_H


#include <cstdint>


#include "base/tools/Buffer.h"
#include "base/tools/Object.h"
#include "crypto/common/HugePagesInfo.h"
#include "crypto/randomx/configuration.h"


struct randomx_cache;


namespace xmrig
{


class RxCache
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(RxCache)

    RxCache(bool hugePages, uint32_t nodeId);
    RxCache(uint8_t *memory);
    ~RxCache();

    inline bool isJIT() const               { return m_jit; }
    inline const Buffer &seed() const       { return m_seed; }
    inline randomx_cache *get() const       { return m_cache; }
    inline size_t size() const              { return maxSize(); }

    bool init(const Buffer &seed);
    HugePagesInfo hugePages() const;

    static inline constexpr size_t maxSize() { return RANDOMX_CACHE_MAX_SIZE; }

private:
    void create(uint8_t *memory);

    bool m_jit              = true;
    Buffer m_seed;
    randomx_cache *m_cache  = nullptr;
    VirtualMemory *m_memory = nullptr;
};


} 


#endif 
