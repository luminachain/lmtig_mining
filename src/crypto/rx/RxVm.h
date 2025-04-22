

#ifndef XMRIG_RX_VM_H
#define XMRIG_RX_VM_H


#include <cstdint>


class randomx_vm;


namespace xmrig
{


class Assembly;
class RxDataset;


class RxVm
{
public:
    static randomx_vm *create(RxDataset *dataset, uint8_t *scratchpad, bool softAes, const Assembly &assembly, uint32_t node);
    static void destroy(randomx_vm *vm);
};


} 


#endif 
