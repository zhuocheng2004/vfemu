
#include <vfemu/constants.h>
#include <modules/tst001.h>

using namespace vfemu;


static VFEMUStatus tst001_init(VFEMUModule* module, void* config) {
	return SUCCESS;
}


static VFEMUStatus tst001_exit(VFEMUModule* module) {
	return SUCCESS;
}


VFEMUModule tst001_module = {
	.min_sdk_version =	VFEMU_VERSION,
	.target_sdk_version = 	VFEMU_VERSION,
	.name =			"tst001",

	.init =			tst001_init,
	.exit =			tst001_exit,

	.num_ports =		0,
	.ports =		nullptr,
};
