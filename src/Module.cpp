
#include <cstring>
#include <list>
#include <vfemu/Module.h>

namespace vfemu {

Registry<ModuleType> ModuleType::registry;

Module::Module(const int num_ports, const std::vector<Port> ports) 
	: num_ports(num_ports) {
	for (auto port : ports) {
		this->ports.push_back(port);
	}
}


Status Module::init(void) {
	return Status::SUCCESS;
}

Status Module::exit(void) {
	return Status::SUCCESS;
}


}
