

#ifndef XMRIG_ARGON2_IMPL_H
#define XMRIG_ARGON2_IMPL_H


namespace xmrig {


class String;


namespace argon2 {


class Impl
{
public:
    static bool select(const String &nameHint, bool benchmark = false);
    static const String &name();
};


}} 


#endif 
