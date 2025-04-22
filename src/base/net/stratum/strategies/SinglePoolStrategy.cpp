


#include "base/net/stratum/strategies/SinglePoolStrategy.h"
#include "3rdparty/rapidjson/document.h"
#include "base/kernel/interfaces/IClient.h"
#include "base/kernel/interfaces/IStrategyListener.h"
#include "base/kernel/Platform.h"
#include "base/net/stratum/Pool.h"


xmrig::SinglePoolStrategy::SinglePoolStrategy(const Pool &pool, int retryPause, int retries, IStrategyListener *listener, bool quiet) :
    m_active(false),
    m_listener(listener)
{
    m_client = pool.createClient(0, this);
    m_client->setRetries(retries);
    m_client->setRetryPause(retryPause * 1000);
    m_client->setQuiet(quiet);
}


xmrig::SinglePoolStrategy::~SinglePoolStrategy()
{
    m_client->deleteLater();
}


int64_t xmrig::SinglePoolStrategy::submit(const JobResult &result)
{
    return m_client->submit(result);
}


void xmrig::SinglePoolStrategy::connect()
{
    m_client->connect();
}


void xmrig::SinglePoolStrategy::resume()
{
    if (!isActive()) {
        return;
    }

    m_listener->onJob(this, m_client, m_client->job(), rapidjson::Value(rapidjson::kNullType));
}


void xmrig::SinglePoolStrategy::setAlgo(const Algorithm &algo)
{
    m_client->setAlgo(algo);
}


void xmrig::SinglePoolStrategy::setProxy(const ProxyUrl &proxy)
{
    m_client->setProxy(proxy);
}


void xmrig::SinglePoolStrategy::stop()
{
    m_client->disconnect();
}


void xmrig::SinglePoolStrategy::tick(uint64_t now)
{
    m_client->tick(now);
}


void xmrig::SinglePoolStrategy::onClose(IClient *, int)
{
    if (!isActive()) {
        return;
    }

    m_active = false;
    m_listener->onPause(this);
}


void xmrig::SinglePoolStrategy::onJobReceived(IClient *client, const Job &job, const rapidjson::Value &params)
{
    m_listener->onJob(this, client, job, params);
}


void xmrig::SinglePoolStrategy::onLogin(IClient *client, rapidjson::Document &doc, rapidjson::Value &params)
{
    m_listener->onLogin(this, client, doc, params);
}


void xmrig::SinglePoolStrategy::onLoginSuccess(IClient *client)
{
    m_active = true;
    m_listener->onActive(this, client);
}


void xmrig::SinglePoolStrategy::onResultAccepted(IClient *client, const SubmitResult &result, const char *error)
{
    m_listener->onResultAccepted(this, client, result, error);
}


void xmrig::SinglePoolStrategy::onVerifyAlgorithm(const IClient *client, const Algorithm &algorithm, bool *ok)
{
    m_listener->onVerifyAlgorithm(this, client, algorithm, ok);
}
