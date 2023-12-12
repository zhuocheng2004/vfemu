
#include <cstring>
#include <list>
#include <vfemu/Module.h>

namespace vfemu {

Registry<ModuleType> ModuleType::registry;

Module::Module(const std::vector<Port> ports) {
	for (auto port : ports) {
		// copy operator invoked here
		this->ports.push_back(port);
	}
}


Status Module::init(void) {
	return Status::SUCCESS;
}

Status Module::exit(void) {
	return Status::SUCCESS;
}

Port* Module::getPort(const char* id) {
	for (auto& port : ports) {
		if (strcmp(id, port.id) == 0) {
			return &port;
		}
	}
	return nullptr;
}

}
