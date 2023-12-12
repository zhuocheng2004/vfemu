
#ifndef VFEMU_MODULES_CHAR1_H
#define VFEMU_MODULES_CHAR1_H

#include <chrono>
#include <thread>
#include <vfemu/Module.h>


namespace vfemu {

namespace char1 {


/*
 * one-char output module
 */

#define CHAR1_OUT "char1_out"


class Char1OutModule : public Module {
public:
	inline Char1OutModule(const std::vector<Port> ports)
		: Module(ports) { }

	inline Status init() {
		return Status::SUCCESS;
	}

	inline Status exit() {
		return Status::SUCCESS;
	}
};

extern Status char1_out_receive(void* data);

extern const std::vector<Port> char1_out_ports;

class Char1OutModuleType : public ModuleType {
/**
 * ports: 
 *	in: pin8
 */
public:
	inline Char1OutModuleType() : ModuleType(CHAR1_OUT, 1, char1_out_ports) {}

	inline Char1OutModule* create() {
		return new Char1OutModule(ports);
	}
};


/*
 * 1-char periodic generator
 */

#define CHAR1_GEN "char1_gen"

class Char1GenConfig {
public:
	const u8			ch;
	const std::chrono::milliseconds	interval;
};


class Char1GenModule : public Module {
public:
	inline Char1GenModule(const std::vector<Port> ports, const Char1GenConfig& config) 
		: Module(ports), 
		  ch(config.ch), interval(config.interval) { }
	
	Status init();

	Status exit();
private:
	const u8			ch;
	const std::chrono::milliseconds	interval;

	bool				running;
	std::thread			*thread;

	static void char1_gen_thread(Char1GenModule* module);
};

extern const std::vector<Port> char1_gen_ports;

class Char1GenModuleType : public ModuleType {
/**
 * ports: 
 *	out: pin8
 */
public:
	inline Char1GenModuleType() : ModuleType(CHAR1_GEN, 1, char1_gen_ports) {}

	inline Char1GenModule* create(const Char1GenConfig& config) {
		return new Char1GenModule(ports, config);
	}
};


}

}


#endif /* VFEMU_MODULES_CHAR1_H */
