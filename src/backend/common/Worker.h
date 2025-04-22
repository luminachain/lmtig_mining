

#ifndef XMRIG_WORKER_H
#define XMRIG_WORKER_H


#include "backend/common/interfaces/IWorker.h"


namespace xmrig {


class Worker : public IWorker
{
public:
    Worker(size_t id, int64_t affinity, int priority);

    size_t threads() const override                         { return 1; }

protected:
    inline int64_t affinity() const                         { return m_affinity; }
    inline size_t id() const override                       { return m_id; }
    inline uint32_t node() const                            { return m_node; }

    uint64_t m_count                = 0;

private:
    const int64_t m_affinity;
    const size_t m_id;
    uint32_t m_node                 = 0;
};


} 


#endif 
