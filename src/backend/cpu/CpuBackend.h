

#ifndef XMRIG_CPUBACKEND_H
#define XMRIG_CPUBACKEND_H


#include "backend/common/interfaces/IBackend.h"
#include "base/tools/Object.h"


#include <utility>


namespace xmrig {


class Controller;
class CpuBackendPrivate;
class Miner;


class CpuBackend : public IBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(CpuBackend)

    CpuBackend(Controller *controller);
    ~CpuBackend() override;

protected:
    inline void execCommand(char) override {}

    bool isEnabled() const override;
    bool isEnabled(const Algorithm &algorithm) const override;
    bool tick(uint64_t ticks) override;
    const Hashrate *hashrate() const override;
    const String &profileName() const override;
    const String &type() const override;
    void prepare(const Job &nextJob) override;
    void printHashrate(bool details) override;
    void printHealth() override;
    void setJob(const Job &job) override;
    void start(IWorker *worker, bool ready) override;
    void stop() override;

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const override;
    void handleRequest(IApiRequest &request) override;
#   endif

#   ifdef XMRIG_FEATURE_BENCHMARK
    Benchmark *benchmark() const override;
    void printBenchProgress() const override;
#   endif

private:
    CpuBackendPrivate *d_ptr;
};


} 


#endif 
