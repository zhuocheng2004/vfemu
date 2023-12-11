
#include <cstring>
#include <list>
#include <vfemu/Module.h>

namespace vfemu {


/**
 * registered module prototypes
 */
static std::list<VFEMUModule*> modules;


VFEMUModule* getModule(const char* name) {
	for (auto module : modules) {
		if (module->name == name) {
			return module;
		}
	}
	return nullptr;
}


VFEMUStatus registerModule(VFEMUModule* module) {
	for (auto _module : modules) {
		if (_module->name == module->name) {
			return ERR_EXIST;
		}
	}
	modules.push_front(module);
	return SUCCESS;
}

VFEMUStatus unregisterModule(VFEMUModule* module) {
	modules.remove(module);
	return SUCCESS;
}


extern VFEMUStatus simple_copy(VFEMUModule* to, VFEMUModule* from) {
	memcpy(to, from, sizeof(VFEMUModule));
	to->ports = new VFEMUPort[from->num_ports];
	for (int i = 0; i < from->num_ports; i++) {
		memcpy(&(to->ports[i]), &(from->ports[i]), sizeof(VFEMUPort));
	}
	return SUCCESS;
}


}
