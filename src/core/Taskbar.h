

#ifndef XMRIG_TASKBAR_H
#define XMRIG_TASKBAR_H


namespace xmrig {


struct TaskbarPrivate;


class Taskbar
{
public:
    Taskbar();
    ~Taskbar();

    void setActive(bool active);
    void setEnabled(bool enabled);

private:
    bool m_active = false;
    bool m_enabled = true;

    TaskbarPrivate* d_ptr = nullptr;

    void updateTaskbarColor();
};


} 


#endif 
