

#include "hw/dmi/DmiReader.h"
#include "hw/dmi/DmiTools.h"


#include <Carbon/Carbon.h>


namespace xmrig {


static int checksum(const uint8_t *buf, size_t len)
{
    uint8_t sum = 0;

    for (size_t a = 0; a < len; a++) {
        sum += buf[a];
    }

    return (sum == 0);
}


static uint8_t *dmi_table(uint32_t base, uint32_t &len, io_service_t service)
{
    CFMutableDictionaryRef properties = nullptr;
    if (IORegistryEntryCreateCFProperties(service, &properties, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess) {
        return nullptr;
    }

    CFDataRef data;
    uint8_t *buf = nullptr;

    if (CFDictionaryGetValueIfPresent(properties, CFSTR("SMBIOS"), (const void **)&data)) {
        assert(len == CFDataGetLength(data));

        len = CFDataGetLength(data);
        buf = reinterpret_cast<uint8_t *>(malloc(len));

        CFDataGetBytes(data, CFRangeMake(0, len), buf);
    }

    CFRelease(properties);

    return buf;
}


static uint8_t *smbios_decode(uint8_t *buf, uint32_t &size, uint32_t &version, io_service_t service)
{
    if (buf[0x05] > 0x20 || !checksum(buf, buf[0x05]) || memcmp(buf + 0x10, "_DMI_", 5) != 0 || !checksum(buf + 0x10, 0x0F))  {
        return nullptr;
    }

    version = ((buf[0x06] << 8) + buf[0x07]) << 8;
    size    = dmi_get<uint16_t>(buf + 0x16);

    return dmi_table(dmi_get<uint32_t>(buf + 0x18), size, service);
}

} 


bool xmrig::DmiReader::read()
{
    mach_port_t port;
    IOMasterPort(MACH_PORT_NULL, &port);

    io_service_t service = IOServiceGetMatchingService(port, IOServiceMatching("AppleSMBIOS"));
    if (service == MACH_PORT_NULL) {
        return false;
    }

    CFDataRef data = reinterpret_cast<CFDataRef>(IORegistryEntryCreateCFProperty(service, CFSTR("SMBIOS-EPS"), kCFAllocatorDefault, kNilOptions));
    if (!data || CFDataGetLength(data) < 0x1f) {
        return false;
    }

    uint8_t buf[0x20]{};
    CFDataGetBytes(data, CFRangeMake(0, sizeof(buf) - 1), buf);
    CFRelease(data);

    auto smb      = smbios_decode(buf, m_size, m_version, service);
    const bool rc = smb ? decode(smb) : false;

    IOObjectRelease(service);

    return rc;
}
