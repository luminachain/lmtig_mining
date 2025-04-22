

#ifndef XMRIG_HTTPRESPONSE_H
#define XMRIG_HTTPRESPONSE_H


#include <cstdint>
#include <map>
#include <string>


namespace xmrig {


class HttpResponse
{
public:
    HttpResponse(uint64_t id, int statusCode = 200);

    inline int statusCode() const                                           { return m_statusCode; }
    inline void setHeader(const std::string &key, const std::string &value) { m_headers.insert({ key, value }); }
    inline void setStatus(int code)                                         { m_statusCode = code; }

    bool isAlive() const;
    void end(const char *data = nullptr, size_t size = 0);

private:
    const uint64_t m_id;
    int m_statusCode;
    std::map<const std::string, const std::string> m_headers;
};


} 


#endif 

