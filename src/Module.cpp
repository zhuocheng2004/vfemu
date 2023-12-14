
#include <list>
#include <vfemu/Module.h>


namespace vfemu {


Module::Module(const std::vector<std::pair<const std::string, Port*>> ports) {
	for (auto port : ports) {
		this->ports.push_back(port);
	}
	initPorts();
}


Status Module::init(void) {
	return Status::SUCCESS;
}

Status Module::exit(void) {
	return Status::SUCCESS;
}


Port* Module::getPort(const std::string& id) {
	for (auto port : ports) {
		if (id == port.first) {
			return port.second;
		}
	}
	return nullptr;
}


Status Module::initPorts() {
	for(auto port: ports) {
		port.second->module = this;
	}
	return Status::SUCCESS;
}


Status initModules(std::vector<Module*> modules) {
	for (auto module : modules) {
		if (module)
			module->init();
	}
	return Status::SUCCESS;
}

Status exitModules(std::vector<Module*> modules) {
	for (auto module : modules) {
		if (module)
			module->exit();
	}
	return Status::SUCCESS;
}

}
