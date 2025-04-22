


#ifndef XMRIG_APIREQUEST_H
#define XMRIG_APIREQUEST_H


#include "base/api/interfaces/IApiRequest.h"
#include "base/tools/String.h"
#include "base/tools/Object.h"


namespace xmrig {


class ApiRequest : public IApiRequest
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(ApiRequest)

    ApiRequest(Source source, bool restricted);
    ~ApiRequest() override;

protected:
    enum State {
        STATE_NEW,
        STATE_ACCEPTED,
        STATE_DONE
    };

    inline bool accept() override                   { m_state = STATE_ACCEPTED; return true; }
    inline bool isDone() const override             { return m_state == STATE_DONE; }
    inline bool isNew() const override              { return m_state == STATE_NEW; }
    inline bool isRestricted() const override       { return m_restricted; }
    inline const String &rpcMethod() const override { return m_rpcMethod; }
    inline int version() const override             { return m_version; }
    inline RequestType type() const override        { return m_type; }
    inline Source source() const override           { return m_source; }
    inline void done(int) override                  { m_state = STATE_DONE; }

    int m_version       = 1;
    RequestType m_type  = REQ_UNKNOWN;
    State m_state       = STATE_NEW;
    String m_rpcMethod;

private:
    const bool m_restricted;
    const Source m_source;
};


} 


#endif 

