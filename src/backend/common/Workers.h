

#ifndef XMRIG_WORKERS_H
#define XMRIG_WORKERS_H


#include <memory>


#include "backend/common/Thread.h"
#include "backend/cpu/CpuLaunchData.h"


#ifdef XMRIG_FEATURE_OPENCL
#   include "backend/opencl/OclLaunchData.h"
#endif


#ifdef XMRIG_FEATURE_CUDA
#   include "backend/cuda/CudaLaunchData.h"
#endif


namespace xmrig {


class Benchmark;
class Hashrate;
class WorkersPrivate;


template<class T>
class Workers
{
public:
    XMRIG_DISABLE_COPY_MOVE(Workers)

    Workers();
    ~Workers();

    inline void start(const std::vector<T> &data)   { start(data, true); }

    bool tick(uint64_t ticks);
    const Hashrate *hashrate() const;
    void jobEarlyNotification(const Job &job);
    void setBackend(IBackend *backend);
    void stop();

#   ifdef XMRIG_FEATURE_BENCHMARK
    void start(const std::vector<T> &data, const std::shared_ptr<Benchmark> &benchmark);
#   endif

private:
    static IWorker *create(Thread<T> *handle);
    static void *onReady(void *arg);

    void start(const std::vector<T> &data, bool sleep);

    std::vector<Thread<T> *> m_workers;
    WorkersPrivate *d_ptr;
};


template<class T>
void xmrig::Workers<T>::jobEarlyNotification(const Job &job)
{
    for (Thread<T>* t : m_workers) {
        if (t->worker()) {
            t->worker()->jobEarlyNotification(job);
        }
    }
}


template<>
IWorker *Workers<CpuLaunchData>::create(Thread<CpuLaunchData> *handle);
extern template class Workers<CpuLaunchData>;


#ifdef XMRIG_FEATURE_OPENCL
template<>
IWorker *Workers<OclLaunchData>::create(Thread<OclLaunchData> *handle);
extern template class Workers<OclLaunchData>;
#endif


#ifdef XMRIG_FEATURE_CUDA
template<>
IWorker *Workers<CudaLaunchData>::create(Thread<CudaLaunchData> *handle);
extern template class Workers<CudaLaunchData>;
#endif


} 

#endif
