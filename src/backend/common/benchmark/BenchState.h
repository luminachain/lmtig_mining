

#ifndef XMRIG_BENCHSTATE_H
#define XMRIG_BENCHSTATE_H


#include <atomic>
#include <cstddef>
#include <cstdint>


namespace xmrig {


class Algorithm;
class IBackend;
class IBenchListener;


class BenchState
{
public:
    static bool isDone();
    static uint32_t size();
    static uint64_t referenceHash(const Algorithm &algo, uint32_t size, uint32_t threads);
    static uint64_t start(size_t threads, const IBackend *backend);
    static void destroy();
    static void done();
    static void init(IBenchListener *listener, uint32_t size);
    static void setSize(uint32_t size);

    inline static uint64_t data()           { return m_data; }
    inline static void add(uint64_t value)  { m_data.fetch_xor(value, std::memory_order_relaxed); }

private:
    static std::atomic<uint64_t> m_data;
};


} 


#endif 
