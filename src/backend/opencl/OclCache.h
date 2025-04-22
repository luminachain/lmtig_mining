

#ifndef XMRIG_OCLCACHE_H
#define XMRIG_OCLCACHE_H


#include <string>


using cl_program = struct _cl_program *;


namespace xmrig {


class IOclRunner;


class OclCache
{
public:
    static cl_program build(const IOclRunner *runner);
    static std::string cacheKey(const char *deviceKey, const char *options, const char *source);
    static std::string cacheKey(const IOclRunner *runner);

private:
    static std::string prefix();
    static void createDirectory();
    static void save(cl_program program, const std::string &fileName);
};


} 


#endif 
