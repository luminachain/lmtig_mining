

#ifndef XMRIG_BASECONFIG_H
#define XMRIG_BASECONFIG_H


#include "base/kernel/config/Title.h"
#include "base/kernel/interfaces/IConfig.h"
#include "base/net/http/Http.h"
#include "base/net/stratum/Pools.h"


#ifdef XMRIG_FEATURE_TLS
#   include "base/net/tls/TlsConfig.h"
#endif


namespace xmrig {


class IJsonReader;


class BaseConfig : public IConfig
{
public:
    static const char *kApi;
    static const char *kApiId;
    static const char *kApiWorkerId;
    static const char *kAutosave;
    static const char *kBackground;
    static const char *kColors;
    static const char *kDryRun;
    static const char *kHttp;
    static const char *kLogFile;
    static const char *kPrintTime;
    static const char *kSyslog;
    static const char *kTitle;
    static const char *kUserAgent;
    static const char *kVerbose;
    static const char *kWatch;

#   ifdef XMRIG_FEATURE_TLS
    static const char *kTls;
#   endif

    BaseConfig() = default;

    inline bool isAutoSave() const                          { return m_autoSave; }
    inline bool isBackground() const                        { return m_background; }
    inline bool isDryRun() const                            { return m_dryRun; }
    inline bool isSyslog() const                            { return m_syslog; }
    inline const char *logFile() const                      { return m_logFile.data(); }
    inline const char *userAgent() const                    { return m_userAgent.data(); }
    inline const Http &http() const                         { return m_http; }
    inline const Pools &pools() const                       { return m_pools; }
    inline const String &apiId() const                      { return m_apiId; }
    inline const String &apiWorkerId() const                { return m_apiWorkerId; }
    inline const Title &title() const                       { return m_title; }
    inline uint32_t printTime() const                       { return m_printTime; }

#   ifdef XMRIG_FEATURE_TLS
    inline const TlsConfig &tls() const                     { return m_tls; }
#   endif

    inline bool isWatch() const override                    { return m_watch && !m_fileName.isNull(); }
    inline const String &fileName() const override          { return m_fileName; }
    inline void setFileName(const char *fileName) override  { m_fileName = fileName; }

    bool read(const IJsonReader &reader, const char *fileName) override;
    bool save() override;

    static void printVersions();

protected:
    bool m_autoSave         = true;
    bool m_background       = false;
    bool m_dryRun           = false;
    bool m_syslog           = false;
    bool m_upgrade          = false;
    bool m_watch            = true;
    Http m_http;
    Pools m_pools;
    String m_apiId;
    String m_apiWorkerId;
    String m_fileName;
    String m_logFile;
    String m_userAgent;
    Title m_title;
    uint32_t m_printTime    = 60;

#   ifdef XMRIG_FEATURE_TLS
    TlsConfig m_tls;
#   endif

private:
    static void setVerbose(const rapidjson::Value &value);
};


} 


#endif 
