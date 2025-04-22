


#include "base/net/http/HttpListener.h"
#include "3rdparty/llhttp/llhttp.h"
#include "base/io/log/Log.h"
#include "base/net/http/HttpData.h"


void xmrig::HttpListener::onHttpData(const HttpData &data)
{
#   ifdef APP_DEBUG
    if (!data.isRequest()) {
        LOG_DEBUG("%s " CYAN_BOLD("http%s:
                  m_tag, data.tlsVersion() ? "s" : "", data.host(), data.port(), llhttp_method_name(static_cast<llhttp_method>(data.method)), data.url.data(),
                  (data.status >= 400 || data.status < 0) ? 31 : 32, data.status, data.body.size());

        if (data.body.size() < (Log::kMaxBufferSize - 1024) && data.isJSON()) {
            Log::print(BLUE_BG_BOLD("%s:") BLACK_BOLD_S " %.*s", data.headers.at(HttpData::kContentTypeL).c_str(), static_cast<int>(data.body.size()), data.body.c_str());
        }
    }
#   endif

    m_listener->onHttpData(data);
}
