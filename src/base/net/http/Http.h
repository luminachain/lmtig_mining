

#ifndef XMRIG_HTTP_H
#define XMRIG_HTTP_H


#include "base/tools/String.h"


namespace xmrig {


class Http
{
public:
    static const char *kEnabled;
    static const char *kHost;
    static const char *kLocalhost;
    static const char *kPort;
    static const char *kRestricted;
    static const char *kToken;

    Http();

    inline bool isAuthRequired() const         { return !m_restricted || !m_token.isNull(); }
    inline bool isEnabled() const              { return m_enabled; }
    inline bool isRestricted() const           { return m_restricted; }
    inline const String &host() const          { return m_host; }
    inline const String &token() const         { return m_token; }
    inline uint16_t port() const               { return m_port; }
    inline void setEnabled(bool enabled)       { m_enabled = enabled; }
    inline void setHost(const char *host)      { m_host = host; }
    inline void setRestricted(bool restricted) { m_restricted = restricted; }
    inline void setToken(const char *token)    { m_token = token; }

    inline bool operator!=(const Http &other) const    { return !isEqual(other); }
    inline bool operator==(const Http &other) const    { return isEqual(other); }

    bool isEqual(const Http &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    void load(const rapidjson::Value &http);
    void setPort(int port);

private:
    bool m_enabled      = false;
    bool m_restricted   = true;
    String m_host;
    String m_token;
    uint16_t m_port     = 0;
};


} 


#endif 

