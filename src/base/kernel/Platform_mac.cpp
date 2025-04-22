


#include <IOKit/IOKitLib.h>
#include <IOKit/ps/IOPowerSources.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <uv.h>
#include <thread>
#include <fstream>


#include "base/kernel/Platform.h"
#include "version.h"


char *xmrig::Platform::createUserAgent()
{
    constexpr const size_t max = 256;

    char *buf = new char[max]();
    int length = snprintf(buf, max,
                          "%s/%s (Macintosh; macOS"
#                         ifdef XMRIG_ARM
                          "; arm64"
#                         else
                          "; x86_64"
#                         endif
                          ") libuv/%s", APP_NAME, APP_VERSION, uv_version_string());

#   ifdef __clang__
    length += snprintf(buf + length, max - length, " clang/%d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#   elif defined(__GNUC__)
    length += snprintf(buf + length, max - length, " gcc/%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   endif

    return buf;
}


bool xmrig::Platform::setThreadAffinity(uint64_t cpu_id)
{
    return true;
}


void xmrig::Platform::setProcessPriority(int)
{
}


void xmrig::Platform::setThreadPriority(int priority)
{
    if (priority == -1) {
        return;
    }

    int prio = 19;
    switch (priority)
    {
    case 1:
        prio = 5;
        break;

    case 2:
        prio = 0;
        break;

    case 3:
        prio = -5;
        break;

    case 4:
        prio = -10;
        break;

    case 5:
        prio = -15;
        break;

    default:
        break;
    }

    setpriority(PRIO_PROCESS, 0, prio);
}


bool xmrig::Platform::isOnBatteryPower()
{
    return IOPSGetTimeRemainingEstimate() != kIOPSTimeRemainingUnlimited;
}


uint64_t xmrig::Platform::idleTime()
{
    uint64_t idle_time  = 0;
    const auto service  = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOHIDSystem"));
    const auto property = IORegistryEntryCreateCFProperty(service, CFSTR("HIDIdleTime"), kCFAllocatorDefault, 0);

    CFNumberGetValue((CFNumberRef)property, kCFNumberSInt64Type, &idle_time);

    CFRelease(property);
    IOObjectRelease(service);

    return idle_time / 1000000U;
}
