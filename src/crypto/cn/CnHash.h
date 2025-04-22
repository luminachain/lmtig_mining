

#ifndef XMRIG_CN_HASH_H
#define XMRIG_CN_HASH_H


#include <cstddef>
#include <cstdint>
#include <map>


#include "crypto/cn/CnAlgo.h"
#include "crypto/common/Assembly.h"


struct cryptonight_ctx;


namespace xmrig
{

using cn_hash_fun     = void (*)(const uint8_t *, size_t, uint8_t *, cryptonight_ctx **, uint64_t);
using cn_mainloop_fun = void (*)(cryptonight_ctx **);


class CnHash
{
public:
    enum AlgoVariant {
        AV_AUTO,        
        AV_SINGLE,      
        AV_DOUBLE,      
        AV_SINGLE_SOFT, 
        AV_DOUBLE_SOFT, 
        AV_TRIPLE,      
        AV_QUAD,        
        AV_PENTA,       
        AV_TRIPLE_SOFT, 
        AV_QUAD_SOFT,   
        AV_PENTA_SOFT,  
        AV_MAX
    };

    CnHash();
    virtual ~CnHash();

    static cn_hash_fun fn(const Algorithm &algorithm, AlgoVariant av, Assembly::Id assembly);

private:
    struct cn_hash_fun_array {
        cn_hash_fun data[AV_MAX][Assembly::MAX];
    };

    std::map<Algorithm, cn_hash_fun_array*> m_map;
};


} 


#endif 
