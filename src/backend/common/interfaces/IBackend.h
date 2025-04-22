

#ifndef XMRIG_IBACKEND_H
#define XMRIG_IBACKEND_H


#include "3rdparty/rapidjson/fwd.h"
#include "base/tools/Object.h"


#include <cstdint>


namespace xmrig {


class Algorithm;
class Benchmark;
class Hashrate;
class IApiRequest;
class IWorker;
class Job;
class String;


class IBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(IBackend)

    IBackend()          = default;
    virtual ~IBackend() = default;

    virtual bool isEnabled() const                                      = 0;
    virtual bool isEnabled(const Algorithm &algorithm) const            = 0;
    virtual bool tick(uint64_t ticks)                                   = 0;
    virtual const Hashrate *hashrate() const                            = 0;
    virtual const String &profileName() const                           = 0;
    virtual const String &type() const                                  = 0;
    virtual void execCommand(char command)                              = 0;
    virtual void prepare(const Job &nextJob)                            = 0;
    virtual void printHashrate(bool details)                            = 0;
    virtual void printHealth()                                          = 0;
    virtual void setJob(const Job &job)                                 = 0;
    virtual void start(IWorker *worker, bool ready)                     = 0;
    virtual void stop()                                                 = 0;

#   ifdef XMRIG_FEATURE_API
    virtual rapidjson::Value toJSON(rapidjson::Document &doc) const     = 0;
    virtual void handleRequest(IApiRequest &request)                    = 0;
#   endif

#   ifdef XMRIG_FEATURE_BENCHMARK
    virtual Benchmark *benchmark() const                                = 0;
    virtual void printBenchProgress() const                             = 0;
#   endif
};


} 


#endif 
