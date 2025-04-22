

#ifndef XMRIG_IDNSLISTENER_H
#define XMRIG_IDNSLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class DnsRecords;


class IDnsListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IDnsListener)

    IDnsListener()          = default;
    virtual ~IDnsListener() = default;

    virtual void onResolved(const DnsRecords &records, int status, const char *error)   = 0;
};


} 


#endif 
