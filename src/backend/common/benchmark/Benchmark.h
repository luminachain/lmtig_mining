

#ifndef XMRIG_BENCHMARK_H
#define XMRIG_BENCHMARK_H


#include "base/tools/Object.h"


namespace xmrig {


class IBackend;


class Benchmark
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Benchmark)

    Benchmark(size_t workers, const IBackend *backend);
    ~Benchmark() = default;

    bool finish(uint64_t totalHashCount);
    void printProgress() const;
    void start();

private:
    const IBackend *m_backend;
    const size_t m_workers;
    uint64_t m_current          = 0;
    uint64_t m_startTime        = 0;
};


} 


#endif 
