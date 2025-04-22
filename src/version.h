

#ifndef XMRIG_VERSION_H
#define XMRIG_VERSION_H

#define APP_ID        "lmtig_mining"
#define APP_NAME      "lmtig_mining"
#define APP_DESC      "lmtig_mining"
#define APP_VERSION   "0.1.82"
#define APP_KIND      "mining"

#define APP_VER_MAJOR  0
#define APP_VER_MINOR  1
#define APP_VER_PATCH  82

#ifdef _MSC_VER
#   if (_MSC_VER >= 1930)
#       define MSVC_VERSION 2022
#   elif (_MSC_VER >= 1920 && _MSC_VER < 1930)
#       define MSVC_VERSION 2019
#   elif (_MSC_VER >= 1910 && _MSC_VER < 1920)
#       define MSVC_VERSION 2017
#   elif _MSC_VER == 1900
#       define MSVC_VERSION 2015
#   elif _MSC_VER == 1800
#       define MSVC_VERSION 2013
#   elif _MSC_VER == 1700
#       define MSVC_VERSION 2012
#   elif _MSC_VER == 1600
#       define MSVC_VERSION 2010
#   else
#       define MSVC_VERSION 0
#   endif
#endif

#ifdef XMRIG_OS_WIN
#    define APP_OS "Windows"
#elif defined XMRIG_OS_IOS
#    define APP_OS "iOS"
#elif defined XMRIG_OS_MACOS
#    define APP_OS "macOS"
#elif defined XMRIG_OS_ANDROID
#    define APP_OS "Android"
#elif defined XMRIG_OS_LINUX
#    define APP_OS "Linux"
#elif defined XMRIG_OS_FREEBSD
#    define APP_OS "FreeBSD"
#else
#    define APP_OS "Unknown OS"
#endif

#define STR(X) #X
#define STR2(X) STR(X)

#ifdef XMRIG_ARM
#   define APP_ARCH "ARMv" STR2(XMRIG_ARM)
#else
#   if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#       define APP_ARCH "x86-64"
#   else
#       define APP_ARCH "x86"
#   endif
#endif

#ifdef XMRIG_64_BIT
#   define APP_BITS "64 bit"
#else
#   define APP_BITS "32 bit"
#endif

#endif
