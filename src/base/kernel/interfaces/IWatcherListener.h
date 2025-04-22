

#ifndef XMRIG_IWATCHERLISTENER_H
#define XMRIG_IWATCHERLISTENER_H


namespace xmrig {


class String;


class IWatcherListener
{
public:
    virtual ~IWatcherListener() = default;

    virtual void onFileChanged(const String &fileName) = 0;
};


} 


#endif 
