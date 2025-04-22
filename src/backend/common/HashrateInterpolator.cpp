


#include "backend/common/HashrateInterpolator.h"


uint64_t xmrig::HashrateInterpolator::interpolate(uint64_t timeStamp) const
{
    timeStamp -= LagMS;

    std::lock_guard<std::mutex> l(m_lock);

    const size_t N = m_data.size();

    if (N < 2) {
        return 0;
    }

    for (size_t i = 0; i < N - 1; ++i) {
        const auto& a = m_data[i];
        const auto& b = m_data[i + 1];

        if (a.second <= timeStamp && timeStamp <= b.second) {
            return a.first + static_cast<int64_t>(b.first - a.first) * (timeStamp - a.second) / (b.second - a.second);
        }
    }

    return 0;
}

void xmrig::HashrateInterpolator::addDataPoint(uint64_t count, uint64_t timeStamp)
{
    std::lock_guard<std::mutex> l(m_lock);

    
    while (!m_data.empty() && (timeStamp - m_data.front().second > LagMS * 2)) {
        m_data.pop_front();
    }

    m_data.emplace_back(count, timeStamp);
}
