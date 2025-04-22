


#include <winsock2.h>
#include <windows.h>
#include <ntsecapi.h>
#include <tchar.h>


#include "crypto/common/VirtualMemory.h"
#include "base/io/log/Log.h"
#include "crypto/common/portable/mm_malloc.h"


#ifdef XMRIG_SECURE_JIT
#   define SECURE_PAGE_EXECUTE_READWRITE PAGE_READWRITE
#else
#   define SECURE_PAGE_EXECUTE_READWRITE PAGE_EXECUTE_READWRITE
#endif


namespace xmrig {


static bool hugepagesAvailable = false;




static BOOL SetLockPagesPrivilege() {
    HANDLE token;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return FALSE;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValue(nullptr, SE_LOCK_MEMORY_NAME, &(tp.Privileges[0].Luid))) {
        return FALSE;
    }

    BOOL rc = AdjustTokenPrivileges(token, FALSE, (PTOKEN_PRIVILEGES) &tp, 0, nullptr, nullptr);
    if (!rc || GetLastError() != ERROR_SUCCESS) {
        return FALSE;
    }

    CloseHandle(token);

    return TRUE;
}


static LSA_UNICODE_STRING StringToLsaUnicodeString(LPCTSTR string) {
    LSA_UNICODE_STRING lsaString;

    const auto dwLen = (DWORD) wcslen(string);
    lsaString.Buffer = (LPWSTR) string;
    lsaString.Length = (USHORT)((dwLen) * sizeof(WCHAR));
    lsaString.MaximumLength = (USHORT)((dwLen + 1) * sizeof(WCHAR));
    return lsaString;
}


static BOOL ObtainLockPagesPrivilege() {
    HANDLE token;
    PTOKEN_USER user = nullptr;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        DWORD size = 0;

        GetTokenInformation(token, TokenUser, nullptr, 0, &size);
        if (size) {
            user = (PTOKEN_USER) LocalAlloc(LPTR, size);
        }

        GetTokenInformation(token, TokenUser, user, size, &size);
        CloseHandle(token);
    }

    if (!user) {
        return FALSE;
    }

    LSA_HANDLE handle;
    LSA_OBJECT_ATTRIBUTES attributes;
    ZeroMemory(&attributes, sizeof(attributes));

    BOOL result = FALSE;
    if (LsaOpenPolicy(nullptr, &attributes, POLICY_ALL_ACCESS, &handle) == 0) {
        LSA_UNICODE_STRING str = StringToLsaUnicodeString(_T(SE_LOCK_MEMORY_NAME));

        if (LsaAddAccountRights(handle, user->User.Sid, &str, 1) == 0) {
            LOG_NOTICE("Huge pages support was successfully enabled, but reboot required to use it");
            result = TRUE;
        }

        LsaClose(handle);
    }

    LocalFree(user);
    return result;
}


static BOOL TrySetLockPagesPrivilege() {
    if (SetLockPagesPrivilege()) {
        return TRUE;
    }

    return ObtainLockPagesPrivilege() && SetLockPagesPrivilege();
}


} 


bool xmrig::VirtualMemory::isHugepagesAvailable()
{
    return hugepagesAvailable;
}


bool xmrig::VirtualMemory::isOneGbPagesAvailable()
{
    return false;
}


bool xmrig::VirtualMemory::protectRW(void *p, size_t size)
{
    DWORD oldProtect;

    return VirtualProtect(p, size, PAGE_READWRITE, &oldProtect) != 0;
}


bool xmrig::VirtualMemory::protectRWX(void *p, size_t size)
{
    DWORD oldProtect;

    return VirtualProtect(p, size, PAGE_EXECUTE_READWRITE, &oldProtect) != 0;
}


bool xmrig::VirtualMemory::protectRX(void *p, size_t size)
{
    DWORD oldProtect;

    return VirtualProtect(p, size, PAGE_EXECUTE_READ, &oldProtect) != 0;
}


void *xmrig::VirtualMemory::allocateExecutableMemory(size_t size, bool hugePages)
{
    void* result = nullptr;

    if (hugePages) {
        result = VirtualAlloc(nullptr, align(size), MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES, SECURE_PAGE_EXECUTE_READWRITE);
    }

    if (!result) {
        result = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, SECURE_PAGE_EXECUTE_READWRITE);
    }

    return result;
}


void *xmrig::VirtualMemory::allocateLargePagesMemory(size_t size)
{
    const size_t min = GetLargePageMinimum();
    void *mem        = nullptr;

    if (min > 0) {
        mem = VirtualAlloc(nullptr, align(size, min), MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES, PAGE_READWRITE);
    }

    return mem;
}


void *xmrig::VirtualMemory::allocateOneGbPagesMemory(size_t)
{
    return nullptr;
}


void xmrig::VirtualMemory::flushInstructionCache(void *p, size_t size)
{
    ::FlushInstructionCache(GetCurrentProcess(), p, size);
}


void xmrig::VirtualMemory::freeLargePagesMemory(void *p, size_t)
{
    VirtualFree(p, 0, MEM_RELEASE);
}


void xmrig::VirtualMemory::osInit(size_t hugePageSize)
{
    if (hugePageSize) {
        hugepagesAvailable = TrySetLockPagesPrivilege();
    }
}


bool xmrig::VirtualMemory::allocateLargePagesMemory()
{
    m_scratchpad = static_cast<uint8_t*>(allocateLargePagesMemory(m_size));
    if (m_scratchpad) {
        m_flags.set(FLAG_HUGEPAGES, true);

        return true;
    }

    return false;
}

bool xmrig::VirtualMemory::allocateOneGbPagesMemory()
{
    m_scratchpad = nullptr;
    return false;
}


void xmrig::VirtualMemory::freeLargePagesMemory()
{
    freeLargePagesMemory(m_scratchpad, m_size);
}
