

#ifndef XMRIG_USAGE_H
#define XMRIG_USAGE_H


#include "version.h"


#include <string>


namespace xmrig {


static inline const std::string &usage()
{
    static std::string u;

    if (!u.empty()) {
        return u;
    }

    u += "Usage: lmtig_mining [OPTIONS]\n\nOptions:\n";
    u += "  -v, --version               output the version number\n";
    u += "  -o, --url<URL>               URL of mining server\n";
    u += "  -a, --algo<ALGO>             mining algorithm\n";
    u += "      --coin<COIN>             specify coin instead of algorithm\n";
    u += "  -u, --user<USERNAME>         username for mining server\n";
    u += "  -p, --pass <char>         password for mining server\n";
    u += "  -h, --help                  display help for command\n";
 

    return u;
}


} 

#endif
