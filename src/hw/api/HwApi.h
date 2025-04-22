

#ifndef XMRIG_HWAPI_H
#define XMRIG_HWAPI_H


#include "base/api/interfaces/IApiListener.h"


#include <memory>


namespace xmrig {


class DmiReader;


class HwApi : public IApiListener
{
public:
    HwApi() = default;

protected:
    void onRequest(IApiRequest &request) override;

private:
#   ifdef XMRIG_FEATURE_DMI
    std::shared_ptr<DmiReader> m_dmi;
#   endif
};


} 


#endif 
