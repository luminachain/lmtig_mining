


#include "backend/common/benchmark/BenchState.h"
#include "backend/common/benchmark/BenchState_test.h"
#include "backend/common/interfaces/IBenchListener.h"
#include "base/io/Async.h"
#include "base/tools/Chrono.h"


#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>


namespace xmrig {


class BenchStatePrivate
{
public:
    BenchStatePrivate(IBenchListener *listener, uint32_t size) :
        listener(listener),
        size(size)
    {}


    IBenchListener *listener;
    std::mutex mutex;
    std::shared_ptr<Async> async;
    uint32_t remaining               = 0;
    uint32_t size;
    uint64_t doneTime                = 0;
};


static BenchStatePrivate *d_ptr = nullptr;
std::atomic<uint64_t> BenchState::m_data{};


} 



bool xmrig::BenchState::isDone()
{
    return d_ptr == nullptr;
}


uint32_t xmrig::BenchState::size()
{
    return d_ptr ? d_ptr->size : 0U;
}


uint64_t xmrig::BenchState::referenceHash(const Algorithm &algo, uint32_t size, uint32_t threads)
{
    uint64_t hash = 0;

    try {
        const auto &h = (threads == 1) ? hashCheck1T : hashCheck;
        hash = h.at(algo).at(size);
    } catch (const std::exception &ex) {}

    return hash;
}


uint64_t xmrig::BenchState::start(size_t threads, const IBackend *backend)
{
    assert(d_ptr != nullptr);

    d_ptr->remaining = static_cast<uint32_t>(threads);

    d_ptr->async = std::make_shared<Async>([] {
        d_ptr->listener->onBenchDone(m_data, 0, d_ptr->doneTime);

        destroy();
    });

    const uint64_t ts = Chrono::steadyMSecs();
    d_ptr->listener->onBenchReady(ts, d_ptr->remaining, backend);

    return ts;
}


void xmrig::BenchState::destroy()
{
    delete d_ptr;
    d_ptr = nullptr;
}


void xmrig::BenchState::done()
{
    assert(d_ptr != nullptr && d_ptr->async && d_ptr->remaining > 0);

    const uint64_t ts = Chrono::steadyMSecs();

    std::lock_guard<std::mutex> lock(d_ptr->mutex);

    d_ptr->doneTime = std::max(d_ptr->doneTime, ts);
    --d_ptr->remaining;

    if (d_ptr->remaining == 0) {
        d_ptr->async->send();
    }
}


void xmrig::BenchState::init(IBenchListener *listener, uint32_t size)
{
    assert(d_ptr == nullptr);

    d_ptr = new BenchStatePrivate(listener, size);
}


void xmrig::BenchState::setSize(uint32_t size)
{
    assert(d_ptr != nullptr);

    d_ptr->size = size;
}
