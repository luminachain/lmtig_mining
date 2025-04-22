

#ifndef XMRIG_ICONFIGLISTENER_H
#define XMRIG_ICONFIGLISTENER_H


namespace xmrig {


class IConfig;


class IConfigListener
{
public:
    virtual ~IConfigListener() = default;

    virtual void onNewConfig(IConfig *config) = 0;
};


} 


#endif 
