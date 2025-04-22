

#ifndef XMRIG_ILINELISTENER_H
#define XMRIG_ILINELISTENER_H


#include "base/tools/Object.h"


#include <cstdint>


namespace xmrig {


class ILineListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(ILineListener)

    ILineListener()             = default;
    virtual ~ILineListener()    = default;

    virtual void onLine(char *line, size_t size) = 0;
};


} 


#endif 
