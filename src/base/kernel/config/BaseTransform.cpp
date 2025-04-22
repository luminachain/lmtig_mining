

#include <cstdio>


#ifdef _MSC_VER
#   include "getopt/getopt.h"
#else
#   include <getopt.h>
#endif


#include "base/kernel/config/BaseTransform.h"
#include "base/io/json/JsonChain.h"
#include "base/io/log/Log.h"
#include "base/kernel/config/BaseConfig.h"
#include "base/kernel/interfaces/IConfig.h"
#include "base/kernel/Process.h"
#include "base/net/dns/DnsConfig.h"
#include "base/net/stratum/Pool.h"
#include "base/net/stratum/Pools.h"
#include "core/config/Config_platform.h"


#ifdef XMRIG_FEATURE_TLS
#   include "base/net/tls/TlsConfig.h"
#endif


void xmrig::BaseTransform::load(JsonChain &chain, Process *process, IConfigTransform &transform)
{
    using namespace rapidjson;

    int key     = 0;
    int argc    = process->arguments().argc();
    char **argv = process->arguments().argv();

    Document doc(kObjectType);

    while (true) {
        key = getopt_long(argc, argv, short_options, options, nullptr); 
        if (key < 0) {
            break;
        }

        if (key == IConfig::ConfigKey) {
            chain.add(std::move(doc));
            chain.addFile(optarg);

            doc = Document(kObjectType);
        }
        else {
            transform.transform(doc, key, optarg);
        }
    }

    if (optind < argc) {
        LOG_WARN("%s: unsupported non-option argument '%s'", argv[0], argv[optind]);
    }

    transform.finalize(doc);
    chain.add(std::move(doc));
}


void xmrig::BaseTransform::finalize(rapidjson::Document &doc)
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    if (m_algorithm.isValid() && doc.HasMember(Pools::kPools)) {
        auto &pools = doc[Pools::kPools];
        for (Value &pool : pools.GetArray()) {
            if (!pool.HasMember(Pool::kAlgo)) {
                pool.AddMember(StringRef(Pool::kAlgo), m_algorithm.toJSON(), allocator);
            }
        }
    }

    if (m_coin.isValid() && doc.HasMember(Pools::kPools)) {
        auto &pools = doc[Pools::kPools];
        for (Value &pool : pools.GetArray()) {
            if (!pool.HasMember(Pool::kCoin)) {
                pool.AddMember(StringRef(Pool::kCoin), m_coin.toJSON(), allocator);
            }
        }
    }

    if (m_http) {
        set(doc, BaseConfig::kHttp, Http::kEnabled, true);
    }
}


void xmrig::BaseTransform::transform(rapidjson::Document &doc, int key, const char *arg)
{
    switch (key) {
    case IConfig::AlgorithmKey: 
        if (!doc.HasMember(Pools::kPools)) {
            m_algorithm = arg;
        }
        else {
            return add(doc, Pools::kPools, Pool::kAlgo, arg);
        }
        break;

    case IConfig::CoinKey: 
        if (!doc.HasMember(Pools::kPools)) {
            m_coin = arg;
        }
        else {
            return add(doc, Pools::kPools, Pool::kCoin, arg);
        }
        break;

    case IConfig::UserpassKey: 
        {
            const char *p = strrchr(arg, ':');
            if (!p) {
                return;
            }

            char *user = new char[p - arg + 1]();
            strncpy(user, arg, static_cast<size_t>(p - arg));

            add<const char *>(doc, Pools::kPools, Pool::kUser, user);
            add(doc, Pools::kPools, Pool::kPass, p + 1);
            delete [] user;
        }
        break;

    case IConfig::UrlKey:    
    case IConfig::StressKey: 
    {
        if (!doc.HasMember(Pools::kPools)) {
            doc.AddMember(rapidjson::StringRef(Pools::kPools), rapidjson::kArrayType, doc.GetAllocator());
        }

        rapidjson::Value &array = doc[Pools::kPools];
        if (array.Size() == 0 || Pool(array[array.Size() - 1]).isValid()) {
            array.PushBack(rapidjson::kObjectType, doc.GetAllocator());
        }

#       ifdef XMRIG_FEATURE_BENCHMARK
        if (key != IConfig::UrlKey) {
            set(doc, array[array.Size() - 1], Pool::kUrl,
#           ifdef XMRIG_FEATURE_TLS
                "stratum+ssl:
#           else
                "randomx.xmrig.com:3333"
#           endif
            );
        } else
#       endif
        {
            set(doc, array[array.Size() - 1], Pool::kUrl, arg);
        }
        break;
    }

    case IConfig::UserKey: 
        return add(doc, Pools::kPools, Pool::kUser, arg);

    case IConfig::PasswordKey: 
        return add(doc, Pools::kPools, Pool::kPass, arg);

    case IConfig::SpendSecretKey: 
        return add(doc, Pools::kPools, Pool::kSpendSecretKey, arg);

    case IConfig::RigIdKey: 
        return add(doc, Pools::kPools, Pool::kRigId, arg);

    case IConfig::FingerprintKey: 
        return add(doc, Pools::kPools, Pool::kFingerprint, arg);

    case IConfig::SelfSelectKey: 
        return add(doc, Pools::kPools, Pool::kSelfSelect, arg);

    case IConfig::ProxyKey: 
        return add(doc, Pools::kPools, Pool::kSOCKS5, arg);

    case IConfig::LogFileKey: 
        return set(doc, BaseConfig::kLogFile, arg);

    case IConfig::HttpAccessTokenKey: 
        m_http = true;
        return set(doc, BaseConfig::kHttp, Http::kToken, arg);

    case IConfig::HttpHostKey: 
        m_http = true;
        return set(doc, BaseConfig::kHttp, Http::kHost, arg);

    case IConfig::ApiWorkerIdKey: 
        return set(doc, BaseConfig::kApi, BaseConfig::kApiWorkerId, arg);

    case IConfig::ApiIdKey: 
        return set(doc, BaseConfig::kApi, BaseConfig::kApiId, arg);

    case IConfig::UserAgentKey: 
        return set(doc, BaseConfig::kUserAgent, arg);

    case IConfig::TitleKey: 
        return set(doc, BaseConfig::kTitle, arg);

#   ifdef XMRIG_FEATURE_TLS
    case IConfig::TlsCertKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kCert, arg);

    case IConfig::TlsCertKeyKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kCertKey, arg);

    case IConfig::TlsDHparamKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kDhparam, arg);

    case IConfig::TlsCiphersKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kCiphers, arg);

    case IConfig::TlsCipherSuitesKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kCipherSuites, arg);

    case IConfig::TlsProtocolsKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kProtocols, arg);

    case IConfig::TlsGenKey: 
        return set(doc, BaseConfig::kTls, TlsConfig::kGen, arg);
#   endif

    case IConfig::RetriesKey:       
    case IConfig::RetryPauseKey:    
    case IConfig::PrintTimeKey:     
    case IConfig::HttpPort:         
    case IConfig::DonateLevelKey:   
    case IConfig::DaemonPollKey:    
    case IConfig::DaemonJobTimeoutKey: 
    case IConfig::DnsTtlKey:        
    case IConfig::DaemonZMQPortKey: 
        return transformUint64(doc, key, static_cast<uint64_t>(strtol(arg, nullptr, 10)));

    case IConfig::BackgroundKey:  
    case IConfig::SyslogKey:      
    case IConfig::KeepAliveKey:   
    case IConfig::NicehashKey:    
    case IConfig::TlsKey:         
    case IConfig::DryRunKey:      
    case IConfig::HttpEnabledKey: 
    case IConfig::DaemonKey:      
    case IConfig::SubmitToOriginKey: 
    case IConfig::VerboseKey:     
    case IConfig::DnsIPv6Key:     
        return transformBoolean(doc, key, true);

    case IConfig::ColorKey:          
    case IConfig::HttpRestrictedKey: 
    case IConfig::NoTitleKey:        
        return transformBoolean(doc, key, false);

    default:
        break;
    }
}


void xmrig::BaseTransform::transformBoolean(rapidjson::Document &doc, int key, bool enable)
{
    switch (key) {
    case IConfig::BackgroundKey: 
        return set(doc, BaseConfig::kBackground, enable);

    case IConfig::SyslogKey: 
        return set(doc, BaseConfig::kSyslog, enable);

    case IConfig::KeepAliveKey: 
        return add(doc, Pools::kPools, Pool::kKeepalive, enable);

    case IConfig::TlsKey: 
        return add(doc, Pools::kPools, Pool::kTls, enable);

    case IConfig::SubmitToOriginKey: 
        return add(doc, Pools::kPools, Pool::kSubmitToOrigin, enable);
#   ifdef XMRIG_FEATURE_HTTP
    case IConfig::DaemonKey: 
        return add(doc, Pools::kPools, Pool::kDaemon, enable);
#   endif

#   ifndef XMRIG_PROXY_PROJECT
    case IConfig::NicehashKey: 
        return add<bool>(doc, Pools::kPools, Pool::kNicehash, enable);
#   endif

    case IConfig::ColorKey: 
        return set(doc, BaseConfig::kColors, enable);

    case IConfig::HttpRestrictedKey: 
        m_http = true;
        return set(doc, BaseConfig::kHttp, Http::kRestricted, enable);

    case IConfig::HttpEnabledKey: 
        m_http = true;
        break;

    case IConfig::DryRunKey: 
        return set(doc, BaseConfig::kDryRun, enable);

    case IConfig::VerboseKey: 
        return set(doc, BaseConfig::kVerbose, enable);

    case IConfig::NoTitleKey: 
        return set(doc, BaseConfig::kTitle, enable);

    case IConfig::DnsIPv6Key: 
        return set(doc, DnsConfig::kField, DnsConfig::kIPv6, enable);

    default:
        break;
    }
}


void xmrig::BaseTransform::transformUint64(rapidjson::Document &doc, int key, uint64_t arg)
{
    switch (key) {
    case IConfig::RetriesKey: 
        return set(doc, Pools::kRetries, arg);

    case IConfig::RetryPauseKey: 
        return set(doc, Pools::kRetryPause, arg);

    case IConfig::DonateLevelKey: 
        return set(doc, Pools::kDonateLevel, arg);

    case IConfig::ProxyDonateKey: 
        return set(doc, Pools::kDonateOverProxy, arg);

    case IConfig::HttpPort: 
        m_http = true;
        return set(doc, BaseConfig::kHttp, Http::kPort, arg);

    case IConfig::PrintTimeKey: 
        return set(doc, BaseConfig::kPrintTime, arg);

    case IConfig::DnsTtlKey: 
        return set(doc, DnsConfig::kField, DnsConfig::kTTL, arg);

#   ifdef XMRIG_FEATURE_HTTP
    case IConfig::DaemonPollKey:  
        return add(doc, Pools::kPools, Pool::kDaemonPollInterval, arg);

    case IConfig::DaemonJobTimeoutKey:  
        return add(doc, Pools::kPools, Pool::kDaemonJobTimeout, arg);

    case IConfig::DaemonZMQPortKey:  
        return add(doc, Pools::kPools, Pool::kDaemonZMQPort, arg);
#   endif

    default:
        break;
    }
}
