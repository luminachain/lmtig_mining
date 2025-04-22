

#ifndef XMRIG_CUDABACKEND_H
#define XMRIG_CUDABACKEND_H


#include <utility>


#include "backend/common/interfaces/IBackend.h"
#include "base/tools/Object.h"


namespace xmrig {


class Controller;
class CudaBackendPrivate;
class Miner;


class CudaBackend : public IBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(CudaBackend)

    CudaBackend(Controller *controller);

    ~CudaBackend() override;

protected:
    bool isEnabled() const override;
    bool isEnabled(const Algorithm &algorithm) const override;
    const Hashrate *hashrate() const override;
    const String &profileName() const override;
    const String &type() const override;
    void execCommand(char command) override;
    void prepare(const Job &nextJob) override;
    void printHashrate(bool details) override;
    void printHealth() override;
    void setJob(const Job &job) override;
    void start(IWorker *worker, bool ready) override;
    void stop() override;
    bool tick(uint64_t ticks) override;

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const override;
    void handleRequest(IApiRequest &request) override;
#   endif

#   ifdef XMRIG_FEATURE_BENCHMARK
    inline Benchmark *benchmark() const override        { return nullptr; }
    inline void printBenchProgress() const override     {}
#   endif

private:
    CudaBackendPrivate *d_ptr;
};


} 


#endif 
