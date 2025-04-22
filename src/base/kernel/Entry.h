

#ifndef XMRIG_ENTRY_H
#define XMRIG_ENTRY_H


namespace xmrig {


class Process;


class Entry
{
public:
    enum Id {
        Default,
        Usage,
        Version,
        Topo,
        Platforms
    };

    static Id get(const Process &process);
    static int exec(const Process &process, Id id);
};


} 


#endif 
