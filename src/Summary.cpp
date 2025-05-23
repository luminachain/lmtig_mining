

#include <cinttypes>
#include <cstdio>
#include <uv.h>


#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"
#include "base/net/stratum/Pool.h"
#include "core/config/Config.h"
#include "core/Controller.h"
#include "crypto/common/Assembly.h"
#include "crypto/common/VirtualMemory.h"
#include "Summary.h"
#include "version.h"


#ifdef XMRIG_FEATURE_DMI
#   include "hw/dmi/DmiReader.h"
#endif


#ifdef XMRIG_ALGO_RANDOMX
#   include "crypto/rx/RxConfig.h"
#endif


namespace xmrig {


#ifdef XMRIG_OS_WIN
static constexpr const char *kHugepagesSupported = GREEN_BOLD("permission granted");
#else
static constexpr const char *kHugepagesSupported = GREEN_BOLD("supported");
#endif


#ifdef XMRIG_FEATURE_ASM
static const char *coloredAsmNames[] = {
    RED_BOLD("none"),
    "auto",
    GREEN_BOLD("intel"),
    GREEN_BOLD("ryzen"),
    GREEN_BOLD("bulldozer")
};


inline static const char *asmName(Assembly::Id assembly)
{
    return coloredAsmNames[assembly];
}
#endif


static void print_pages(const Config *config)
{
    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") "%s",
               "HUGE PAGES", config->cpu().isHugePages() ? (VirtualMemory::isHugepagesAvailable() ? kHugepagesSupported : RED_BOLD("unavailable")) : RED_BOLD("disabled"));

#   ifdef XMRIG_ALGO_RANDOMX
#   ifdef XMRIG_OS_LINUX
    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") "%s",
               "1GB PAGES", (VirtualMemory::isOneGbPagesAvailable() ? (config->rx().isOneGbPages() ? kHugepagesSupported : YELLOW_BOLD("disabled")) : YELLOW_BOLD("unavailable")));
#   else
    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") "%s", "1GB PAGES", YELLOW_BOLD("unavailable"));
#   endif
#   endif
}


static void print_cpu(const Config *)
{
    const auto info = Cpu::info();

    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s%s (%zu)") " %s %sAES%s",
               "CPU",
               info->brand(),
               info->packages(),
               ICpuInfo::is64bit()    ? GREEN_BOLD("64-bit") : RED_BOLD("32-bit"),
               info->hasAES()         ? GREEN_BOLD_S : RED_BOLD_S "-",
               info->isVM()           ? RED_BOLD_S " VM" : ""
               );
#   if defined(XMRIG_FEATURE_HWLOC)
    Log::print(WHITE_BOLD("   %-13s") BLACK_BOLD("L2:") WHITE_BOLD("%.1f MB") BLACK_BOLD(" L3:") WHITE_BOLD("%.1f MB")
               CYAN_BOLD(" %zu") "C" BLACK_BOLD("/") CYAN_BOLD("%zu") "T"
               BLACK_BOLD(" NUMA:") CYAN_BOLD("%zu"),
               "",
               info->L2() / 1048576.0,
               info->L3() / 1048576.0,
               info->cores(),
               info->threads(),
               info->nodes()
               );
#   else
    Log::print(WHITE_BOLD("   %-13s") BLACK_BOLD("threads:") CYAN_BOLD("%zu"), "", info->threads());
#   endif
}


static void print_memory(const Config *config)
{
    constexpr size_t oneGiB = 1024U * 1024U * 1024U;
    const auto freeMem      = static_cast<double>(uv_get_free_memory());
    const auto totalMem     = static_cast<double>(uv_get_total_memory());

    const double percent = freeMem > 0 ? ((totalMem - freeMem) / totalMem * 100.0) : 100.0;

    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") CYAN_BOLD("%.1f/%.1f") CYAN(" GB") BLACK_BOLD(" (%.0f%%)"),
               "MEMORY",
               (totalMem - freeMem) / oneGiB,
               totalMem / oneGiB,
               percent
               );

#   ifdef XMRIG_FEATURE_DMI
    if (!config->isDMI()) {
        return;
    }

    DmiReader reader;
    if (!reader.read()) {
        return;
    }

    const bool printEmpty = reader.memory().size() <= 8;

    for (const auto &memory : reader.memory()) {
        if (!memory.isValid()) {
            continue;
        }

        if (memory.size()) {
            Log::print(WHITE_BOLD("   %-13s") "%s: " CYAN_BOLD("%" PRIu64) CYAN(" GB ") WHITE_BOLD("%s @ %" PRIu64 " MHz ") BLACK_BOLD("%s"),
                       "", memory.id().data(), memory.size() / oneGiB, memory.type(), memory.speed() / 1000000ULL, memory.product().data());
        }
        else if (printEmpty) {
            Log::print(WHITE_BOLD("   %-13s") "%s: " BLACK_BOLD("<empty>"), "", memory.id().data());
        }
    }

    const auto &board = Cpu::info()->isVM() ? reader.system() : reader.board();

    if (board.isValid()) {
        Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") WHITE_BOLD("%s") " - " WHITE_BOLD("%s"), "MOTHERBOARD", board.vendor().data(), board.product().data());
    }
#   endif
}


static void print_threads(const Config *config)
{
    Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s") WHITE_BOLD("%s%d%%"),
               "DONATE",
               config->pools().donateLevel() == 0 ? RED_BOLD_S : "",
               config->pools().donateLevel()
               );

#   ifdef XMRIG_FEATURE_ASM
    if (config->cpu().assembly() == Assembly::AUTO) {
        const Assembly assembly = Cpu::info()->assembly();

        Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13sauto:%s"), "ASSEMBLY", asmName(assembly));
    }
    else {
        Log::print(GREEN_BOLD(" * ") WHITE_BOLD("%-13s%s"), "ASSEMBLY", asmName(config->cpu().assembly()));
    }
#   endif
}


static void print_commands(Config *)
{
    if (Log::isColors()) {
        Log::print(GREEN_BOLD(" * ") WHITE_BOLD("COMMANDS     ") MAGENTA_BG_BOLD("h") WHITE_BOLD("ashrate, ")
                                                                 MAGENTA_BG_BOLD("p") WHITE_BOLD("ause, ")
                                                                 MAGENTA_BG_BOLD("r") WHITE_BOLD("esume, ")
                                                                 WHITE_BOLD("re") MAGENTA_BG(WHITE_BOLD_S "s") WHITE_BOLD("ults, ")
                                                                 MAGENTA_BG_BOLD("c") WHITE_BOLD("onnection")
                   );
    }
    else {
        Log::print(" * COMMANDS     'h' hashrate, 'p' pause, 'r' resume, 's' results, 'c' connection");
    }
}


} 


void xmrig::Summary::print(Controller *controller)
{
    const auto config = controller->config();

    config->printVersions();
    print_pages(config);
    print_cpu(config);
    print_memory(config);
    print_threads(config);
    config->pools().print();

    print_commands(config);
}



