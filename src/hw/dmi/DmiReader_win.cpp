


#include "hw/dmi/DmiReader.h"
#include "hw/dmi/DmiTools.h"


#include <windows.h>


namespace xmrig {



struct RawSMBIOSData {
    uint8_t	Used20CallingMethod;
    uint8_t	SMBIOSMajorVersion;
    uint8_t	SMBIOSMinorVersion;
    uint8_t	DmiRevision;
    uint32_t Length;
    uint8_t	SMBIOSTableData[];
};


} 


bool xmrig::DmiReader::read()
{
    constexpr uint32_t RSMB = 0x52534D42;

    const uint32_t size = GetSystemFirmwareTable(RSMB, 0, nullptr, 0);
    auto smb            = reinterpret_cast<RawSMBIOSData *>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size));

    if (!smb) {
        return false;
    }

    if (GetSystemFirmwareTable(RSMB, 0, smb, size) != size) {
        HeapFree(GetProcessHeap(), 0, smb);

        return false;
    }

    m_version = (smb->SMBIOSMajorVersion << 16) + (smb->SMBIOSMinorVersion << 8) + smb->DmiRevision;
    m_size    = smb->Length;

    return decode(smb->SMBIOSTableData, [smb]() {
        HeapFree(GetProcessHeap(), 0, smb);
    });
}
