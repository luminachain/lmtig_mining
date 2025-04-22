

#ifndef XMRIG_RX_BASICSTORAGE_H
#define XMRIG_RX_BASICSTORAGE_H


#include "backend/common/interfaces/IRxStorage.h"


namespace xmrig
{


class RxBasicStoragePrivate;


class RxBasicStorage : public IRxStorage
{
public:
    XMRIG_DISABLE_COPY_MOVE(RxBasicStorage);

    RxBasicStorage();
    ~RxBasicStorage() override;

protected:
    bool isAllocated() const override;
    HugePagesInfo hugePages() const override;
    RxDataset *dataset(const Job &job, uint32_t nodeId) const override;
    void init(const RxSeed &seed, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority) override;

private:
    RxBasicStoragePrivate *d_ptr;
};


} 


#endif 
