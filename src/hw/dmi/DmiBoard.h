

#ifndef XMRIG_DMIBOARD_H
#define XMRIG_DMIBOARD_H


#include "base/tools/String.h"


namespace xmrig {


struct dmi_header;


class DmiBoard
{
public:
    DmiBoard() = default;

    inline const String &product() const    { return m_product; }
    inline const String &vendor() const     { return m_vendor; }
    inline bool isValid() const             { return !m_product.isEmpty() && !m_vendor.isEmpty(); }

    void decode(dmi_header *h);

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
#   endif

private:
    String m_product;
    String m_vendor;
};


} 


#endif 
