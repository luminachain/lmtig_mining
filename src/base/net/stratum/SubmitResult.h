

#ifndef XMRIG_SUBMITRESULT_H
#define XMRIG_SUBMITRESULT_H


#include "base/tools/Chrono.h"


namespace xmrig {


class SubmitResult
{
public:
    SubmitResult() = default;

    inline SubmitResult(int64_t seq, uint64_t diff, uint64_t actualDiff, int64_t reqId, uint32_t backend) :
        reqId(reqId),
        seq(seq),
        backend(backend),
        actualDiff(actualDiff),
        diff(diff),
        m_start(Chrono::steadyMSecs())
    {}

    inline void done() { elapsed = Chrono::steadyMSecs() - m_start; }

    int64_t reqId           = 0;
    int64_t seq             = 0;
    uint32_t backend        = 0;
    uint64_t actualDiff     = 0;
    uint64_t diff           = 0;
    uint64_t elapsed        = 0;

private:
    uint64_t m_start        = 0;
};


} 


#endif 
