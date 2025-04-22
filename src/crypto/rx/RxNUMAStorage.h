

#ifndef XMRIG_RX_NUMASTORAGE_H
#define XMRIG_RX_NUMASTORAGE_H


#include "backend/common/interfaces/IRxStorage.h"


#include <vector>


namespace xmrig
{


class RxNUMAStoragePrivate;


class RxNUMAStorage : public IRxStorage
{
public:
    XMRIG_DISABLE_COPY_MOVE(RxNUMAStorage);

    RxNUMAStorage(const std::vector<uint32_t> &nodeset);
    ~RxNUMAStorage() override;

protected:
    bool isAllocated() const override;
    HugePagesInfo hugePages() const override;
    RxDataset *dataset(const Job &job, uint32_t nodeId) const override;
    void init(const RxSeed &seed, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority) override;

private:
    RxNUMAStoragePrivate *d_ptr;
};


} 


#endif 
