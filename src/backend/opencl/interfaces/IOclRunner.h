

#ifndef XMRIG_IOCLRUNNER_H
#define XMRIG_IOCLRUNNER_H


#include "base/tools/Object.h"


#include <cstdint>


using cl_context = struct _cl_context *;


namespace xmrig {


class Algorithm;
class Job;
class OclLaunchData;


class IOclRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE(IOclRunner)

    IOclRunner()          = default;
    virtual ~IOclRunner() = default;

    virtual cl_context ctx() const                          = 0;
    virtual const Algorithm &algorithm() const              = 0;
    virtual const char *buildOptions() const                = 0;
    virtual const char *deviceKey() const                   = 0;
    virtual const char *source() const                      = 0;
    virtual const OclLaunchData &data() const               = 0;
    virtual size_t intensity() const                        = 0;
    virtual size_t threadId() const                         = 0;
    virtual uint32_t roundSize() const                      = 0;
    virtual uint32_t processedHashes() const                = 0;
    virtual uint32_t deviceIndex() const                    = 0;
    virtual void build()                                    = 0;
    virtual void init()                                     = 0;
    virtual void run(uint32_t nonce, uint32_t nonce_offset, uint32_t *hashOutput)  = 0;
    virtual void set(const Job &job, uint8_t *blob)         = 0;
    virtual void jobEarlyNotification(const Job&)           = 0;

protected:
    virtual size_t bufferSize() const                       = 0;
};


} 


#endif 
