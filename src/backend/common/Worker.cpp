

#include "backend/common/Worker.h"
#include "base/kernel/Platform.h"
#include "crypto/common/VirtualMemory.h"


xmrig::Worker::Worker(size_t id, int64_t affinity, int priority) :
    m_affinity(affinity),
    m_id(id)
{
    m_node = VirtualMemory::bindToNUMANode(affinity);

    Platform::trySetThreadAffinity(affinity);
    Platform::setThreadPriority(priority);
}
