

#ifndef XMRIG_OCLSOURCE_H
#define XMRIG_OCLSOURCE_H


namespace xmrig {


class Algorithm;


class OclSource
{
public:
    static const char *get(const Algorithm &algorithm);
    static void init();
};


} 


#endif 
