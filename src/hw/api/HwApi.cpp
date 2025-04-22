


#include "hw/api/HwApi.h"
#include "base/api/interfaces/IApiRequest.h"
#include "base/tools/String.h"


#ifdef XMRIG_FEATURE_DMI
#   include "hw/dmi/DmiReader.h"
#endif


void xmrig::HwApi::onRequest(IApiRequest &request)
{
    if (request.method() == IApiRequest::METHOD_GET) {
#       ifdef XMRIG_FEATURE_DMI
        if (request.url() == "/2/dmi") {
            if (!m_dmi) {
                m_dmi = std::make_shared<DmiReader>();
                m_dmi->read();
            }

            request.accept();
            m_dmi->toJSON(request.reply(), request.doc());
        }
#       endif
    }
}
