

#ifndef XMRIG_OCLGENERATOR_H
#define XMRIG_OCLGENERATOR_H


namespace xmrig {


class Algorithm;
class OclDevice;
class OclThreads;


using ocl_gen_config_fun = bool (*)(const OclDevice &device, const Algorithm &algorithm, OclThreads &threads);


} 


#endif 
