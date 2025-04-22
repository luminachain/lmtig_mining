

#ifndef XMRIG_CPU_H
#define XMRIG_CPU_H


#include "backend/cpu/interfaces/ICpuInfo.h"


namespace xmrig {


class Cpu
{
public:
    static ICpuInfo *info();
    static rapidjson::Value toJSON(rapidjson::Document &doc);
    static void release();

    inline static Assembly::Id assembly(Assembly::Id hint) { return hint == Assembly::AUTO ? Cpu::info()->assembly() : hint; }
};


} 


#endif 
