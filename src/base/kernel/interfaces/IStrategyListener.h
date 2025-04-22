

#ifndef XMRIG_ISTRATEGYLISTENER_H
#define XMRIG_ISTRATEGYLISTENER_H


#include "3rdparty/rapidjson/fwd.h"
#include "base/tools/Object.h"


namespace xmrig {


class Algorithm;
class IClient;
class IStrategy;
class Job;
class SubmitResult;


class IStrategyListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IStrategyListener);

    IStrategyListener()             = default;
    virtual ~IStrategyListener()    = default;

    virtual void onActive(IStrategy *strategy, IClient *client)                                                        = 0;
    virtual void onJob(IStrategy *strategy, IClient *client, const Job &job, const rapidjson::Value &params)           = 0;
    virtual void onLogin(IStrategy *strategy, IClient *client, rapidjson::Document &doc, rapidjson::Value &params)     = 0;
    virtual void onPause(IStrategy *strategy)                                                                          = 0;
    virtual void onResultAccepted(IStrategy *strategy, IClient *client, const SubmitResult &result, const char *error) = 0;
    virtual void onVerifyAlgorithm(IStrategy *strategy, const IClient *client, const Algorithm &algorithm, bool *ok)   = 0;
};


} 


#endif 
