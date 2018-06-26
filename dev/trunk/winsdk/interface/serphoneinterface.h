
#ifndef __SERPHONE_INTERFACE_H_
#define __SERPHONE_INTERFACE_H_
namespace CcpClientYTX{
struct _SerphoneCoreVTable;
typedef struct _SerphoneVTable SerphoneCoreVTable;
typedef void (*PrintConsoleHook)(int loglevel,const char *);
class ServiceCore; 


ServiceCore *serphone_core_new(const SerphoneCoreVTable *vtable,
						const char *config_path, const char *factory_config, void* userdata);
void serphone_core_destroy(ServiceCore *lc);

extern void PrintConsole(char *fi, int li,char *function,int level,const char * fmt,...);
extern void init_print_log();
extern void uninit_print_log();
}//end namespace CcpClientYTX
#endif