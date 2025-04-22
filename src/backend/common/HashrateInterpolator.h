

#ifndef XMRIG_HASHRATE_INTERPOLATOR_H
#define XMRIG_HASHRATE_INTERPOLATOR_H


#include <mutex>
#include <deque>
#include <utility>


namespace xmrig {


class HashrateInterpolator
{
public:
    enum {
        LagMS = 4000,
    };

    uint64_t interpolate(uint64_t timeStamp) const;
    void addDataPoint(uint64_t count, uint64_t timeStamp);

private:
    
    mutable std::mutex m_lock;
    std::deque<std::pair<uint64_t, uint64_t> > m_data;
};


} 


#endif 
