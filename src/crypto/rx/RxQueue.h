

#ifndef XMRIG_RX_QUEUE_H
#define XMRIG_RX_QUEUE_H


#include "base/kernel/interfaces/IAsyncListener.h"
#include "base/tools/Object.h"
#include "crypto/common/HugePagesInfo.h"
#include "crypto/rx/RxConfig.h"
#include "crypto/rx/RxSeed.h"


#include <condition_variable>
#include <mutex>
#include <thread>


namespace xmrig
{


class IRxListener;
class IRxStorage;
class RxDataset;


class RxQueueItem
{
public:
    RxQueueItem(const RxSeed &seed, const std::vector<uint32_t> &nodeset, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority) :
        hugePages(hugePages),
        oneGbPages(oneGbPages),
        priority(priority),
        mode(mode),
        seed(seed),
        nodeset(nodeset),
        threads(threads)
    {}

    const bool hugePages;
    const bool oneGbPages;
    const int priority;
    const RxConfig::Mode mode;
    const RxSeed seed;
    const std::vector<uint32_t> nodeset;
    const uint32_t threads;
};


class RxQueue : public IAsyncListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(RxQueue);

    RxQueue(IRxListener *listener);
    ~RxQueue() override;

    HugePagesInfo hugePages();
    RxDataset *dataset(const Job &job, uint32_t nodeId);
    template<typename T> bool isReady(const T &seed);
    void enqueue(const RxSeed &seed, const std::vector<uint32_t> &nodeset, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority);

protected:
    inline void onAsync() override  { onReady(); }

private:
    enum State {
        STATE_IDLE,
        STATE_PENDING,
        STATE_SHUTDOWN
    };

    template<typename T> bool isReadyUnsafe(const T &seed) const;
    void backgroundInit();
    void onReady();

    IRxListener *m_listener = nullptr;
    IRxStorage *m_storage   = nullptr;
    RxSeed m_seed;
    State m_state = STATE_IDLE;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::shared_ptr<Async> m_async;
    std::thread m_thread;
    std::vector<RxQueueItem> m_queue;
};


} 


#endif 
