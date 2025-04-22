


#include <algorithm>
#include <uv.h>


#include "base/tools/Arguments.h"


xmrig::Arguments::Arguments(int argc, char **argv) :
    m_argv(argv),
    m_argc(argc)
{
    uv_setup_args(argc, argv);

    for (size_t i = 0; i < static_cast<size_t>(argc); ++i) {
        add(argv[i]);
    }
}


bool xmrig::Arguments::hasArg(const char *name) const
{
    if (m_argc == 1) {
        return false;
    }

    return std::find(m_data.begin() + 1, m_data.end(), name) != m_data.end();
}


const char *xmrig::Arguments::value(const char *key1, const char *key2) const
{
    const size_t size = m_data.size();
    if (size < 3) {
        return nullptr;
    }

    for (size_t i = 1; i < size - 1; ++i) {
        if (m_data[i] == key1 || (key2 && m_data[i] == key2)) {
            return m_data[i + 1];
        }
    }

    return nullptr;
}


void xmrig::Arguments::add(const char *arg)
{
    if (arg == nullptr) {
        return;
    }

    const size_t size = strlen(arg);
    if (size > 4 && arg[0] == '-' && arg[1] == '-') {
        const char *p = strchr(arg, '=');

        if (p) {
            const auto keySize = static_cast<size_t>(p - arg);

            m_data.emplace_back(arg, keySize);
            m_data.emplace_back(arg + keySize + 1);

            return;
        }
    }

    m_data.emplace_back(arg);
}
