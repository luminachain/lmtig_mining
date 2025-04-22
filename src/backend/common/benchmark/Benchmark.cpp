

#include "backend/common/benchmark/Benchmark.h"
#include "backend/common/benchmark/BenchState.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/tools/Chrono.h"


#include <cinttypes>


xmrig::Benchmark::Benchmark(size_t workers, const IBackend *backend) :
    m_backend(backend),
    m_workers(workers)
{
}


bool xmrig::Benchmark::finish(uint64_t totalHashCount)
{
    m_current = totalHashCount;

    return BenchState::isDone();
}


void xmrig::Benchmark::start()
{
    m_startTime = BenchState::start(m_workers, m_backend);
}


void xmrig::Benchmark::printProgress() const
{
    if (!m_startTime || !m_current) {
        return;
    }

    const double dt      = static_cast<double>(Chrono::steadyMSecs() - m_startTime) / 1000.0;
    const double percent = static_cast<double>(m_current) / BenchState::size() * 100.0;

    LOG_NOTICE("%s " MAGENTA_BOLD("%5.2f%% ") CYAN_BOLD("%" PRIu64) CYAN("/%u") BLACK_BOLD(" (%.3fs)"), Tags::bench(), percent, m_current, BenchState::size(), dt);
}
