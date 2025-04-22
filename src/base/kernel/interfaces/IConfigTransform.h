

#ifndef XMRIG_ICONFIGTRANSFORM_H
#define XMRIG_ICONFIGTRANSFORM_H


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class IJsonReader;
class String;


class IConfigTransform
{
public:
    virtual ~IConfigTransform() = default;

    virtual void finalize(rapidjson::Document &doc)                             = 0;
    virtual void transform(rapidjson::Document &doc, int key, const char *arg)  = 0;
};


} 


#endif 
