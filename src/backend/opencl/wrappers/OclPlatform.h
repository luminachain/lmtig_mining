

#ifndef XMRIG_OCLPLATFORM_H
#define XMRIG_OCLPLATFORM_H


#include <vector>


#include "backend/opencl/wrappers/OclDevice.h"
#include "base/tools/String.h"


using cl_platform_id = struct _cl_platform_id *;


namespace xmrig {


class OclPlatform
{
public:
    OclPlatform() = default;
    OclPlatform(size_t index, cl_platform_id id) : m_id(id), m_index(index) {}

    static std::vector<OclPlatform> get();
    static void print();

    inline bool isValid() const      { return m_id != nullptr; }
    inline cl_platform_id id() const { return m_id; }
    inline size_t index() const      { return m_index; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    std::vector<OclDevice> devices() const;
    String extensions() const;
    String name() const;
    String profile() const;
    String vendor() const;
    String version() const;

private:
    cl_platform_id m_id = nullptr;
    size_t m_index      = 0;
};


} 


#endif 
