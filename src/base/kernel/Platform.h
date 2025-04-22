

#ifndef XMRIG_PLATFORM_H
#define XMRIG_PLATFORM_H


#include <cstdint>


#include "base/tools/String.h"


namespace xmrig {


class Platform
{
public:
    static inline bool trySetThreadAffinity(int64_t cpu_id)
    {
        if (cpu_id < 0) {
            return false;
        }

        return setThreadAffinity(static_cast<uint64_t>(cpu_id));
    }

    static bool setThreadAffinity(uint64_t cpu_id);
    static void init(const char *userAgent);
    static void setProcessPriority(int priority);
    static void setThreadPriority(int priority);

    static inline bool isUserActive(uint64_t ms)    { return idleTime() < ms; }
    static inline const String &userAgent()         { return m_userAgent; }

#   ifdef XMRIG_OS_WIN
    static bool hasKeepalive();
#   else
    static constexpr bool hasKeepalive()            { return true; }
#   endif

    static bool isOnBatteryPower();
    static uint64_t idleTime();

private:
    static char *createUserAgent();

    static String m_userAgent;
};


} 


#endif 
