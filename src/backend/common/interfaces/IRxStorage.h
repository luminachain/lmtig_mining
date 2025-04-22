

#ifndef XMRIG_IRXSTORAGE_H
#define XMRIG_IRXSTORAGE_H


#include "base/tools/Object.h"
#include "crypto/common/HugePagesInfo.h"
#include "crypto/rx/RxConfig.h"


#include <cstdint>
#include <utility>


namespace xmrig {


class Job;
class RxDataset;
class RxSeed;


class IRxStorage
{
public:
    XMRIG_DISABLE_COPY_MOVE(IRxStorage)

    IRxStorage()            = default;
    virtual ~IRxStorage()   = default;

    virtual bool isAllocated() const                                                                                            = 0;
    virtual HugePagesInfo hugePages() const                                                                                     = 0;
    virtual RxDataset *dataset(const Job &job, uint32_t nodeId) const                                                           = 0;
    virtual void init(const RxSeed &seed, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority) = 0;
};


} 


#endif 
