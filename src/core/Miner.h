

#ifndef XMRIG_MINER_H
#define XMRIG_MINER_H


#include <vector>


#include "backend/common/interfaces/IRxListener.h"
#include "base/api/interfaces/IApiListener.h"
#include "base/crypto/Algorithm.h"
#include "base/kernel/interfaces/IBaseListener.h"
#include "base/kernel/interfaces/ITimerListener.h"
#include "base/tools/Object.h"


namespace xmrig {


class Controller;
class Job;
class MinerPrivate;
class IBackend;


class Miner : public ITimerListener, public IBaseListener, public IApiListener, public IRxListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Miner)

    Miner(Controller *controller);
    ~Miner() override;

    bool isEnabled() const;
    bool isEnabled(const Algorithm &algorithm) const;
    const Algorithms &algorithms() const;
    const std::vector<IBackend *> &backends() const;
    Job job() const;
    void execCommand(char command);
    void pause();
    void setEnabled(bool enabled);
    void setJob(const Job &job, bool donate);
    void stop();

protected:
    void onConfigChanged(Config *config, Config *previousConfig) override;
    void onTimer(const Timer *timer) override;

#   ifdef XMRIG_FEATURE_API
    void onRequest(IApiRequest &request) override;
#   endif

#   ifdef XMRIG_ALGO_RANDOMX
    void onDatasetReady() override;
#   endif

private:
    MinerPrivate *d_ptr;
};


} 


#endif 
