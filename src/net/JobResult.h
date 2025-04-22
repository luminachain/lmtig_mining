

#ifndef XMRIG_JOBRESULT_H
#define XMRIG_JOBRESULT_H


#include <memory.h>
#include <cstdint>


#include "base/tools/String.h"
#include "base/net/stratum/Job.h"


namespace xmrig {


class JobResult
{
public:
    JobResult() = delete;

    inline JobResult(const Job &job, uint64_t nonce, const uint8_t *result, const uint8_t* header_hash = nullptr, const uint8_t *mix_hash = nullptr, const uint8_t* miner_signature = nullptr) :
        algorithm(job.algorithm()),
        index(job.index()),
        clientId(job.clientId()),
        jobId(job.id()),
        backend(job.backend()),
        nonce(nonce),
        diff(job.diff())
    {
        memcpy(m_result, result, sizeof(m_result));

        if (header_hash) {
            memcpy(m_headerHash, header_hash, sizeof(m_headerHash));
        }

        if (mix_hash) {
            memcpy(m_mixHash, mix_hash, sizeof(m_mixHash));
        }

        if (miner_signature) {
            m_hasMinerSignature = true;
            memcpy(m_minerSignature, miner_signature, sizeof(m_minerSignature));
        }
    }

    inline JobResult(const Job &job) :
        algorithm(job.algorithm()),
        index(job.index()),
        clientId(job.clientId()),
        jobId(job.id()),
        backend(job.backend()),
        nonce(0),
        diff(0)
    {
    }

    inline const uint8_t *result() const     { return m_result; }
    inline uint64_t actualDiff() const       { return Job::toDiff(reinterpret_cast<const uint64_t*>(m_result)[3]); }
    inline uint8_t *result()                 { return m_result; }
    inline const uint8_t *headerHash() const { return m_headerHash; }
    inline const uint8_t *mixHash() const    { return m_mixHash; }

    inline const uint8_t *minerSignature() const { return m_hasMinerSignature ? m_minerSignature : nullptr; }

    const Algorithm algorithm;
    const uint8_t index;
    const String clientId;
    const String jobId;
    const uint32_t backend;
    const uint64_t nonce;
    const uint64_t diff;

private:
    uint8_t m_result[32]     = { 0 };
    uint8_t m_headerHash[32] = { 0 };
    uint8_t m_mixHash[32]    = { 0 };

    uint8_t m_minerSignature[64] = { 0 };
    bool m_hasMinerSignature = false;
};


} 


#endif 
