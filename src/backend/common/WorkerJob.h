

#ifndef XMRIG_WORKERJOB_H
#define XMRIG_WORKERJOB_H


#include <cstring>


#include "base/net/stratum/Job.h"
#include "base/tools/Alignment.h"
#include "crypto/common/Nonce.h"


namespace xmrig {


template<size_t N>
class WorkerJob
{
public:
    inline const Job &currentJob() const    { return m_jobs[index()]; }
    inline uint32_t *nonce(size_t i = 0)    { return reinterpret_cast<uint32_t*>(blob() + (i * currentJob().size()) + nonceOffset()); }
    inline uint64_t sequence() const        { return m_sequence; }
    inline uint8_t *blob()                  { return m_blobs[index()]; }
    inline uint8_t index() const            { return m_index; }


    inline void add(const Job &job, uint32_t reserveCount, Nonce::Backend backend)
    {
        m_sequence = Nonce::sequence(backend);

        if (currentJob() == job) {
            return;
        }

        if (index() == 1 && job.index() == 0 && job == m_jobs[0]) {
            m_index = 0;
            return;
        }

        save(job, reserveCount, backend);
    }


    inline bool nextRound(uint32_t rounds, uint32_t roundSize)
    {
        m_rounds[index()]++;

        if ((m_rounds[index()] & (rounds - 1)) == 0) {
            for (size_t i = 0; i < N; ++i) {
                if (!Nonce::next(index(), nonce(i), rounds * roundSize, nonceMask())) {
                    return false;
                }
            }
        }
        else {
            for (size_t i = 0; i < N; ++i) {
                writeUnaligned(nonce(i), readUnaligned(nonce(i)) + roundSize);
            }
        }

        return true;
    }


    inline int32_t nonceOffset() const { return currentJob().nonceOffset(); }
    inline size_t nonceSize() const { return currentJob().nonceSize(); }

private:
    inline uint64_t nonceMask() const     { return m_nonce_mask[index()]; }

    inline void save(const Job &job, uint32_t reserveCount, Nonce::Backend backend)
    {
        m_index           = job.index();
        const size_t size = job.size();
        m_jobs[index()]   = job;
        m_rounds[index()] = 0;
        m_nonce_mask[index()] = job.nonceMask();

        m_jobs[index()].setBackend(backend);

        for (size_t i = 0; i < N; ++i) {
            memcpy(m_blobs[index()] + (i * size), job.blob(), size);
            Nonce::next(index(), nonce(i), reserveCount, nonceMask());
        }
    }


    alignas(8) uint8_t m_blobs[2][Job::kMaxBlobSize * N]{};
    Job m_jobs[2];
    uint32_t m_rounds[2] = { 0, 0 };
    uint64_t m_nonce_mask[2] = { 0, 0 };
    uint64_t m_sequence  = 0;
    uint8_t m_index      = 0;
};


template<>
inline uint32_t *xmrig::WorkerJob<1>::nonce(size_t)
{
    return reinterpret_cast<uint32_t*>(blob() + nonceOffset());
}


template<>
inline bool xmrig::WorkerJob<1>::nextRound(uint32_t rounds, uint32_t roundSize)
{
    m_rounds[index()]++;

    uint32_t* n = nonce();

    if ((m_rounds[index()] & (rounds - 1)) == 0) {
        if (!Nonce::next(index(), n, rounds * roundSize, nonceMask())) {
            return false;
        }
        if (nonceSize() == sizeof(uint64_t)) {
            writeUnaligned(m_jobs[index()].nonce() + 1, readUnaligned(n + 1));
        }
    }
    else {
        writeUnaligned(n, readUnaligned(n) + roundSize);
    }

    return true;
}


template<>
inline void xmrig::WorkerJob<1>::save(const Job &job, uint32_t reserveCount, Nonce::Backend backend)
{
    m_index           = job.index();
    m_jobs[index()]   = job;
    m_rounds[index()] = 0;
    m_nonce_mask[index()] = job.nonceMask();

    m_jobs[index()].setBackend(backend);

    memcpy(blob(), job.blob(), job.size());
    Nonce::next(index(), nonce(), reserveCount, nonceMask());
}


} 


#endif 
