


#include "crypto/common/HugePagesInfo.h"
#include "crypto/common/VirtualMemory.h"


xmrig::HugePagesInfo::HugePagesInfo(const VirtualMemory *memory)
{
    if (memory->isOneGbPages()) {
        size        = VirtualMemory::align(memory->size(), VirtualMemory::kOneGiB);
        total       = size / VirtualMemory::kOneGiB;
        allocated   = size / VirtualMemory::kOneGiB;
    }
    else {
        size        = VirtualMemory::alignToHugePageSize(memory->size());
        total       = size / VirtualMemory::hugePageSize();
        allocated   = memory->isHugePages() ? total : 0;
    }
}
