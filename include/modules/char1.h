
#ifndef VFEMU_MODULES_CHAR1_H
#define VFEMU_MODULES_CHAR1_H

#include <chrono>
#include <thread>
#include <vfemu/Module.h>


namespace vfemu {

namespace char1 {


#define CHAR1_OUT "char1_out"


class Char1OutModule : public Module {
public:
	inline Char1OutModule(const int num_ports, const std::vector<Port> ports)
		: Module(num_ports, ports) { }

	inline Status init() {
		ports[0].module = this;
		return Status::SUCCESS;
	}

	inline Status exit() {
		ports[0].module = nullptr;
		return Status::SUCCESS;
	}

	static Status char1_out_receive(Module& from, Module& to, void* data);
};

extern const std::vector<Port> char1_out_ports;

class Char1OutModuleType : public ModuleType {
/**
 * ports: 
 *	in: pin8
 */
public:
	inline Char1OutModuleType() : ModuleType(CHAR1_OUT, 1, char1_out_ports) {}

	inline Char1OutModule* create(ModuleConfig* config) {
		return new Char1OutModule(num_ports, ports);
	}
};


class Char1GenConfig : public ModuleConfig {
public:
	const u8			ch;
	const std::chrono::milliseconds	interval;
};


}

}


#endif /* VFEMU_MODULES_CHAR1_H */
