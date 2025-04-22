

#include "base/net/http/HttpApiResponse.h"
#include "3rdparty/rapidjson/prettywriter.h"
#include "3rdparty/rapidjson/stringbuffer.h"
#include "base/net/http/HttpData.h"


namespace xmrig {

static const char *kError  = "error";
static const char *kStatus = "status";

} 


xmrig::HttpApiResponse::HttpApiResponse(uint64_t id) :
    HttpResponse(id),
    m_doc(rapidjson::kObjectType)
{
}


xmrig::HttpApiResponse::HttpApiResponse(uint64_t id, int status) :
    HttpResponse(id),
    m_doc(rapidjson::kObjectType)
{
    setStatus(status);
}


void xmrig::HttpApiResponse::end()
{
    using namespace rapidjson;

    setHeader("Access-Control-Allow-Origin", "*");
    setHeader("Access-Control-Allow-Methods", "GET, PUT, POST, DELETE");
    setHeader("Access-Control-Allow-Headers", "Authorization, Content-Type");

    if (statusCode() >= 400) {
        if (!m_doc.HasMember(kStatus)) {
            m_doc.AddMember(StringRef(kStatus), statusCode(), m_doc.GetAllocator());
        }

        if (!m_doc.HasMember(kError)) {
            m_doc.AddMember(StringRef(kError), StringRef(HttpData::statusName(statusCode())), m_doc.GetAllocator());
        }
    }

    if (m_doc.IsObject() && m_doc.ObjectEmpty()) {
        return HttpResponse::end();
    }

    setHeader(HttpData::kContentType, HttpData::kApplicationJson);

    StringBuffer buffer(nullptr, 4096);
    PrettyWriter<StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(10);
    writer.SetFormatOptions(kFormatSingleLineArray);

    m_doc.Accept(writer);

    HttpResponse::end(buffer.GetString(), buffer.GetSize());
}
