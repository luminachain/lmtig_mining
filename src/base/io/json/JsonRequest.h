

#ifndef XMRIG_JSONREQUEST_H
#define XMRIG_JSONREQUEST_H


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class JsonRequest
{
public:
    static const char *k2_0;
    static const char *kId;
    static const char *kJsonRPC;
    static const char *kMethod;
    static const char *kOK;
    static const char *kParams;
    static const char *kResult;
    static const char *kStatus;

    static const char *kParseError;
    static const char *kInvalidRequest;
    static const char *kMethodNotFound;
    static const char *kInvalidParams;
    static const char *kInternalError;

    constexpr static int kParseErrorCode        = -32700;
    constexpr static int kInvalidRequestCode    = -32600;
    constexpr static int kMethodNotFoundCode    = -32601;
    constexpr static int kInvalidParamsCode     = -32602;
    constexpr static int kInternalErrorCode     = -32603;

    static rapidjson::Document create(const char *method);
    static rapidjson::Document create(int64_t id, const char *method);
    static uint64_t create(rapidjson::Document &doc, const char *method, rapidjson::Value &params);
    static uint64_t create(rapidjson::Document &doc, int64_t id, const char *method, rapidjson::Value &params);
};


} 


#endif 
