

#ifndef XMRIG_IRXLISTENER_H
#define XMRIG_IRXLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class IRxListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IRxListener)

    IRxListener()           = default;
    virtual ~IRxListener()  = default;

#   ifdef XMRIG_ALGO_RANDOMX
    virtual void onDatasetReady() = 0;
#   endif
};


} 


#endif 
