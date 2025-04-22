

#ifndef XMRIG_BATON_H
#define XMRIG_BATON_H


namespace xmrig {


template<typename REQ>
class Baton
{
public:
    inline Baton() { req.data = this; }

    REQ req;
};


} 


#endif 
