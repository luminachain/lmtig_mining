


#include "base/kernel/config/Title.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/Env.h"
#include "version.h"


xmrig::Title::Title(const rapidjson::Value &value)
{
    if (value.IsBool()) {
        m_enabled = value.GetBool();
    }
    else if (value.IsString()) {
        m_value = value.GetString();
    }
}


rapidjson::Value xmrig::Title::toJSON() const
{
    if (isEnabled() && !m_value.isNull()) {
        return m_value.toJSON();
    }

    return rapidjson::Value(m_enabled);
}


xmrig::String xmrig::Title::value() const
{
    if (!isEnabled()) {
        return {};
    }

    if (m_value.isNull()) {
        return APP_NAME " " APP_VERSION;
    }

    return Env::expand(m_value);
}
