#include "cxwindow.h"
#include <cxscript_system.h>
#include "cxfile_system.h"
#include "cxlua.h"

#include "cxvector.h"

using namespace cx;

int main(int argc, char *argv[])
{
	handle_command_args(argc,argv);
	FileSystem::InitWorkPath();

	script_system_prepare_init();
	script_system_run_main_script();
	 

	return 0;
}
