

#ifndef XMRIG_RXFIX_H
#define XMRIG_RXFIX_H


#include <utility>


namespace xmrig
{


class RxFix
{
public:
    static void setMainLoopBounds(const std::pair<const void *, const void *> &bounds);
    static void setupMainLoopExceptionFrame();
};


} 


#endif 
