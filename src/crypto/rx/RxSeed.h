

#ifndef XMRIG_RX_SEED_H
#define XMRIG_RX_SEED_H


#include "base/net/stratum/Job.h"
#include "base/tools/Buffer.h"


namespace xmrig
{


class RxSeed;


class RxSeed
{
public:
    RxSeed() = default;

    inline RxSeed(const Algorithm &algorithm, const Buffer &seed) : m_algorithm(algorithm), m_data(seed)    {}
    inline RxSeed(const Job &job) : m_algorithm(job.algorithm()), m_data(job.seed())                        {}

    inline bool isEqual(const Job &job) const           { return m_algorithm == job.algorithm() && m_data == job.seed(); }
    inline bool isEqual(const RxSeed &other) const      { return m_algorithm == other.m_algorithm && m_data == other.m_data; }
    inline const Algorithm &algorithm() const           { return m_algorithm; }
    inline const Buffer &data() const                   { return m_data; }

    inline bool operator!=(const Job &job) const        { return !isEqual(job); }
    inline bool operator!=(const RxSeed &other) const   { return !isEqual(other); }
    inline bool operator==(const Job &job) const        { return isEqual(job); }
    inline bool operator==(const RxSeed &other) const   { return isEqual(other); }

private:
    Algorithm m_algorithm;
    Buffer m_data;
};


} 


#endif 
