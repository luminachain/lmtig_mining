


#include "base/api/requests/ApiRequest.h"


xmrig::ApiRequest::ApiRequest(Source source, bool restricted) :
    m_restricted(restricted),
    m_source(source)
{
}


xmrig::ApiRequest::~ApiRequest() = default;
