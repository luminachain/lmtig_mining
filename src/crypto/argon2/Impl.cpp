


#include "3rdparty/argon2.h"
#include "base/tools/String.h"
#include "crypto/argon2/Impl.h"



namespace xmrig {


static bool selected = false;
static String implName;


} 


extern "C" {


extern int xmrig_ar2_check_avx512f();
extern int xmrig_ar2_check_avx2();
extern int xmrig_ar2_check_ssse3();
extern int xmrig_ar2_check_sse2();


}


bool xmrig::argon2::Impl::select(const String &nameHint, bool benchmark)
{
    if (!selected) {
#       if defined(__x86_64__) || defined(_M_AMD64)
        auto hint = nameHint;

        if (hint.isEmpty() && !benchmark) {
            if (xmrig_ar2_check_avx512f()) {
                hint = "AVX-512F";
            }
            else if (xmrig_ar2_check_avx2()) {
                hint = "AVX2";
            }
            else if (xmrig_ar2_check_ssse3()) {
                hint = "SSSE3";
            }
            else if (xmrig_ar2_check_sse2()) {
                hint = "SSE2";
            }
        }

        if (!hint.isEmpty()) {
            argon2_select_impl_by_name(hint);
        }
#       endif

        selected = true;
        implName = argon2_get_impl_name();

        return true;
    }

    return false;
}


const xmrig::String &xmrig::argon2::Impl::name()
{
    return implName;
}
