

#ifndef XMRIG_ENV_H
#define XMRIG_ENV_H


#include "base/tools/String.h"


#include <map>


namespace xmrig {


class Env
{
public:
    static String expand(const char *in, const std::map<String, String> &extra = {});
    static String get(const String &name, const std::map<String, String> &extra = {});
    static String hostname();
};


} 


#endif 
