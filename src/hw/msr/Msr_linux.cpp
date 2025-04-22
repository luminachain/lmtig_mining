


#include "hw/msr/Msr.h"
#include "3rdparty/fmt/core.h"
#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"


#include <array>
#include <cctype>
#include <cinttypes>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace xmrig {


static int msr_open(int32_t cpu, int flags)
{
    const auto name = fmt::format("/dev/cpu/{}/msr", cpu < 0 ? Cpu::info()->units().front() : cpu);

    return open(name.c_str(), flags);
}


class MsrPrivate
{
public:
    inline MsrPrivate() : m_available(msr_allow_writes() || msr_modprobe()) {}

    inline bool isAvailable() const { return m_available; }

private:
    inline bool msr_allow_writes()
    {
        std::ofstream file("/sys/module/msr/parameters/allow_writes", std::ios::out | std::ios::binary | std::ios::trunc);
        if (file.is_open()) {
            file << "on";
        }

        return file.good();
    }

    inline bool msr_modprobe()
    {
        return system("/sbin/modprobe msr allow_writes=on > /dev/null 2>&1") == 0;
    }

    const bool m_available;
};


} 


xmrig::Msr::Msr() : d_ptr(new MsrPrivate())
{
    if (!isAvailable()) {
        LOG_WARN("%s " YELLOW_BOLD("msr kernel module is not available"), tag());
    }
}


xmrig::Msr::~Msr()
{
    delete d_ptr;
}


bool xmrig::Msr::isAvailable() const
{
    return d_ptr->isAvailable();
}


bool xmrig::Msr::write(Callback &&callback)
{
    const auto &units = Cpu::info()->units();

    for (int32_t pu : units) {
        if (!callback(pu)) {
            return false;
        }
    }

    return true;
}


bool xmrig::Msr::rdmsr(uint32_t reg, int32_t cpu, uint64_t &value) const
{
    const int fd = msr_open(cpu, O_RDONLY);

    if (fd < 0) {
        return false;
    }

    const bool success = pread(fd, &value, sizeof value, reg) == sizeof value;
    close(fd);

    return success;
}


bool xmrig::Msr::wrmsr(uint32_t reg, uint64_t value, int32_t cpu)
{
    const int fd = msr_open(cpu, O_WRONLY);

    if (fd < 0) {
        return false;
    }

    const bool success = pwrite(fd, &value, sizeof value, reg) == sizeof value;

    close(fd);

    return success;
}
