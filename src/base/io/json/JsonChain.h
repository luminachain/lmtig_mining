

#ifndef XMRIG_JSONCHAIN_H
#define XMRIG_JSONCHAIN_H


#include <vector>


#include "3rdparty/rapidjson/document.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "base/tools/String.h"


namespace xmrig {


class JsonChain : public IJsonReader
{
public:
    JsonChain();

    bool add(rapidjson::Document &&doc);
    bool addFile(const char *fileName);
    bool addRaw(const char *json);

    void dump(const char *fileName);

    inline const String &fileName() const { return m_fileName; }
    inline size_t size() const            { return m_chain.size(); }

protected:
    inline bool isEmpty() const override  { return m_chain.empty(); }

    bool getBool(const char *key, bool defaultValue = false) const override;
    const char *getString(const char *key, const char *defaultValue = nullptr) const override;
    const rapidjson::Value &getArray(const char *key) const override;
    const rapidjson::Value &getObject(const char *key) const override;
    const rapidjson::Value &getValue(const char *key) const override;
    const rapidjson::Value &object() const override;
    double getDouble(const char *key, double defaultValue = 0) const override;
    int getInt(const char *key, int defaultValue = 0) const override;
    int64_t getInt64(const char *key, int64_t defaultValue = 0) const override;
    String getString(const char *key, size_t maxSize) const override;
    uint64_t getUint64(const char *key, uint64_t defaultValue = 0) const override;
    unsigned getUint(const char *key, unsigned defaultValue = 0) const override;

private:
    std::vector<rapidjson::Document> m_chain;
    String m_fileName;
};


} 


#endif 
