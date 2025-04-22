

#ifndef XMRIG_ICLIENTLISTENER_H
#define XMRIG_ICLIENTLISTENER_H


#include <cstdint>


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class Algorithm;
class IClient;
class Job;
class SubmitResult;


class IClientListener
{
public:
    virtual ~IClientListener() = default;

    virtual void onClose(IClient *client, int failures)                                           = 0;
    virtual void onJobReceived(IClient *client, const Job &job, const rapidjson::Value &params)   = 0;
    virtual void onLogin(IClient *client, rapidjson::Document &doc, rapidjson::Value &params)     = 0;
    virtual void onLoginSuccess(IClient *client)                                                  = 0;
    virtual void onResultAccepted(IClient *client, const SubmitResult &result, const char *error) = 0;
    virtual void onVerifyAlgorithm(const IClient *client, const Algorithm &algorithm, bool *ok)   = 0;
};


} 


#endif 
