
/*
 * toy one-char output module
 *
 * char1_out: output whenever data is reveiced
 * Ports:
 * 	in:	pin8	# data in
 * 
 * cchar1_out: output when signaled.
 * Ports:
 * 	in:	pin8	# data in
 * 	ctrl:	pin1	# signal to output data
 */

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
#define CCHAR1_OUT "cchar_out"


class Char1OutModule : public Module {
public:
	inline Char1OutModule(const std::vector<Port> ports)
		: Module(ports) { }

	static Status out_receive(Module* receiver, void* data);
};


class Char1OutModuleType : public ModuleType {
/**
 * ports: 
 *	in: pin8
 */
public:
	static std::vector<Port> char1_out_ports;

	inline Char1OutModuleType() : ModuleType(CHAR1_OUT, char1_out_ports) {}

	inline Char1OutModule* create() {
		return new Char1OutModule(ports);
	}
};


class CChar1OutModule : public Module {
public:
	inline CChar1OutModule(const std::vector<Port> ports)
		: Module(ports), data(0) { }

	static Status out_receive(Module* receiver, void* data);
	static Status ctrl_receive(Module* receiver, void* data);
private:
	u8	data;
};


class CChar1OutModuleType : public ModuleType {
/**
 * ports: 
 *	in: pin8
 */
public:
	static std::vector<Port> cchar1_out_ports;

	inline CChar1OutModuleType() : ModuleType(CCHAR1_OUT, cchar1_out_ports) {}

	inline CChar1OutModule* create() {
		return new CChar1OutModule(ports);
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
	
	virtual Status init();

	virtual Status exit();
private:
	const u8			ch;
	const std::chrono::milliseconds	interval;

	bool				running;
	std::thread			*thread;

	static void char1_gen_thread(Char1GenModule* module);
};


class Char1GenModuleType : public ModuleType {
/**
 * ports: 
 *	out: pin8
 */
public:
	static std::vector<Port> char1_gen_ports;

	inline Char1GenModuleType() : ModuleType(CHAR1_GEN, char1_gen_ports) { }

	inline Char1GenModule* create(const Char1GenConfig& config) {
		return new Char1GenModule(ports, config);
	}
};


}

}


#endif /* VFEMU_MODULES_CHAR1_H */
