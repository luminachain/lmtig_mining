

#ifndef XMRIG_ISTRATEGY_H
#define XMRIG_ISTRATEGY_H


#include <cstdint>


namespace xmrig {


class Algorithm;
class IClient;
class JobResult;
class ProxyUrl;


class IStrategy
{
public:
    virtual ~IStrategy() = default;

    virtual bool isActive() const                      = 0;
    virtual IClient *client() const                    = 0;
    virtual int64_t submit(const JobResult &result)    = 0;
    virtual void connect()                             = 0;
    virtual void resume()                              = 0;
    virtual void setAlgo(const Algorithm &algo)        = 0;
    virtual void setProxy(const ProxyUrl &proxy)       = 0;
    virtual void stop()                                = 0;
    virtual void tick(uint64_t now)                    = 0;
};


} 


#endif 
