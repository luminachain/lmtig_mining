


#include "base/net/tools/NetBuffer.h"
#include "base/kernel/constants.h"
#include "base/net/tools/MemPool.h"


#include <cassert>
#include <uv.h>


namespace xmrig {


static MemPool<XMRIG_NET_BUFFER_CHUNK_SIZE, XMRIG_NET_BUFFER_INIT_CHUNKS> *pool = nullptr;


inline MemPool<XMRIG_NET_BUFFER_CHUNK_SIZE, XMRIG_NET_BUFFER_INIT_CHUNKS> *getPool()
{
    if (!pool) {
        pool = new MemPool<XMRIG_NET_BUFFER_CHUNK_SIZE, XMRIG_NET_BUFFER_INIT_CHUNKS>();
    }

    return pool;
}


} 


char *xmrig::NetBuffer::allocate()
{
    return getPool()->allocate();
}


void xmrig::NetBuffer::destroy()
{
    if (!pool) {
        return;
    }

    assert(pool->freeSize() == pool->size());

    delete pool;
    pool = nullptr;
}


void xmrig::NetBuffer::onAlloc(uv_handle_t *, size_t, uv_buf_t *buf)
{
    buf->base = getPool()->allocate();
    buf->len  = XMRIG_NET_BUFFER_CHUNK_SIZE;
}


void xmrig::NetBuffer::release(const char *buf)
{
    if (buf == nullptr) {
        return;
    }

    getPool()->deallocate(buf);
}


void xmrig::NetBuffer::release(const uv_buf_t *buf)
{
    if (buf->base == nullptr) {
        return;
    }

    getPool()->deallocate(buf->base);
}
