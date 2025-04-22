

#ifndef XMRIG_PROCESS_H
#define XMRIG_PROCESS_H


#include "base/tools/Arguments.h"


#ifdef WIN32
#   define XMRIG_DIR_SEPARATOR "\\"
#else
#   define XMRIG_DIR_SEPARATOR "/"
#endif


namespace xmrig {


class Process
{
public:
    enum Location {
        ExeLocation,
        CwdLocation,
        DataLocation,
        HomeLocation,
        TempLocation
    };

    Process(int argc, char **argv);

    static int pid();
    static int ppid();
    static String exepath();
    static String location(Location location, const char *fileName = nullptr);

    inline const Arguments &arguments() const { return m_arguments; }

private:
    Arguments m_arguments;
};


} 


#endif 
