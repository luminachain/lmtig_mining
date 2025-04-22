

#ifndef XMRIG_THREADS_H
#define XMRIG_THREADS_H


#include <map>
#include <set>


#include "3rdparty/rapidjson/fwd.h"
#include "base/crypto/Algorithm.h"
#include "base/tools/String.h"


namespace xmrig {


template <class T>
class Threads
{
public:
    inline bool has(const char *profile) const                                         { return m_profiles.count(profile) > 0; }
    inline bool isDisabled(const Algorithm &algo) const                                { return m_disabled.count(algo) > 0; }
    inline bool isEmpty() const                                                        { return m_profiles.empty(); }
    inline bool isExist(const Algorithm &algo) const                                   { return isDisabled(algo) || m_aliases.count(algo) > 0 || has(algo.name()); }
    inline const T &get(const Algorithm &algo, bool strict = false) const              { return get(profileName(algo, strict)); }
    inline void disable(const Algorithm &algo)                                         { m_disabled.insert(algo); }
    inline void setAlias(const Algorithm &algo, const char *profile)                   { m_aliases[algo] = profile; }

    inline size_t move(const char *profile, T &&threads)
    {
        if (has(profile)) {
            return 0;
        }

        const size_t count = threads.count();

        if (!threads.isEmpty()) {
            m_profiles.insert({ profile, std::move(threads) });
        }

        return count;
    }

    const T &get(const String &profileName) const;
    size_t read(const rapidjson::Value &value);
    String profileName(const Algorithm &algorithm, bool strict = false) const;
    void toJSON(rapidjson::Value &out, rapidjson::Document &doc) const;

private:
    std::map<Algorithm, String> m_aliases;
    std::map<String, T> m_profiles;
    std::set<Algorithm> m_disabled;
};


} 


#endif 
