

#include "core/Taskbar.h"

#ifdef _WIN32


#include <Shobjidl.h>
#include <Objbase.h>


namespace xmrig {


struct TaskbarPrivate
{
    TaskbarPrivate()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (hr < 0) {
            return;
        }

        hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_taskbar));
        if (hr < 0) {
            return;
        }

        hr = m_taskbar->HrInit();
        if (hr < 0) {
            m_taskbar->Release();
            m_taskbar = nullptr;
            return;
        }

        m_consoleWnd = GetConsoleWindow();
    }

    ~TaskbarPrivate()
    {
        if (m_taskbar) {
            m_taskbar->Release();
        }
        CoUninitialize();
    }

    ITaskbarList3* m_taskbar = nullptr;
    HWND m_consoleWnd = nullptr;
};


Taskbar::Taskbar() : d_ptr(new TaskbarPrivate())
{
}


Taskbar::~Taskbar()
{
    delete d_ptr;
}


void Taskbar::setActive(bool active)
{
    m_active = active;
    updateTaskbarColor();
}


void Taskbar::setEnabled(bool enabled)
{
    m_enabled = enabled;
    updateTaskbarColor();
}


void Taskbar::updateTaskbarColor()
{
    if (d_ptr->m_taskbar) {
        if (m_active) {
            d_ptr->m_taskbar->SetProgressState(d_ptr->m_consoleWnd, m_enabled ? TBPF_NOPROGRESS : TBPF_PAUSED);
            d_ptr->m_taskbar->SetProgressValue(d_ptr->m_consoleWnd, m_enabled ? 0 : 1, 1);
        }
        else {
            d_ptr->m_taskbar->SetProgressState(d_ptr->m_consoleWnd, TBPF_ERROR);
            d_ptr->m_taskbar->SetProgressValue(d_ptr->m_consoleWnd, 1, 1);
        }
    }
}


} 


#else 


namespace xmrig {


Taskbar::Taskbar() {}
Taskbar::~Taskbar() {}
void Taskbar::setActive(bool) {}
void Taskbar::setEnabled(bool) {}


} 


#endif 
