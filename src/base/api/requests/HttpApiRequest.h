


#ifndef XMRIG_HTTPAPIREQUEST_H
#define XMRIG_HTTPAPIREQUEST_H


#include "base/api/requests/ApiRequest.h"
#include "base/net/http/HttpApiResponse.h"
#include "base/tools/String.h"


namespace xmrig {


class HttpData;


class HttpApiRequest : public ApiRequest
{
public:
    HttpApiRequest(const HttpData &req, bool restricted);

protected:
    inline bool hasParseError() const override           { return m_parsed == 2; }
    inline const String &url() const override            { return m_url; }
    inline rapidjson::Document &doc() override           { return m_res.doc(); }
    inline rapidjson::Value &reply() override            { return m_res.doc(); }

    bool accept() override;
    const rapidjson::Value &json() const override;
    Method method() const override;
    void done(int status) override;
    void setRpcError(int code, const char *message = nullptr) override;
    void setRpcResult(rapidjson::Value &result) override;

private:
    void rpcDone(const char *key, rapidjson::Value &value);

    const HttpData &m_req;
    HttpApiResponse m_res;
    int m_parsed = 0;
    rapidjson::Document m_body;
    String m_url;
};


} 


#endif 

