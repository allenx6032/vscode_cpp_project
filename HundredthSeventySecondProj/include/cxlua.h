#include "cxany.h"
#include "cxlua_bind.h"
#include "cxlua_net.h"
#include "cxnet_thread_queue.h"
#include "cxutils.h"
#include <time.h>
#include <ctype.h>
#include <asio.hpp>


void init_default_cwd(const char* arg0);
std::string  get_default_cwd();
void handle_command_args(int argc, char* argv[]);
const char* command_arg_get(const char* key);
const char* command_arg_opt_str(const char* key, const char* def);
int command_arg_opt_int(const char* key, int def);
void luaopen_cxlua(lua_State* L);

 