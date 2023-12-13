
/*
 * toy data input module
 *
 * gen1: 1-bit generator
 * Ports:
 * 	out:	pin1	# data out
 * 
 * gen8: 8-bit generator
 * Ports:
 * 	out:	pin8	# data out
 */

#ifndef VFEMU_MODULES_GEN_H
#define VFEMU_MODULES_GEN_H

#include <vfemu/Module.h>
#include <modules/pin.h>

namespace vfemu {

namespace gen {


#define GEN1 "gen1"
#define GEN8 "gen8"


/* ================
 * 1-bit generator
 */


class Gen1Module : public Module {
public:
	inline Gen1Module(const std::vector<Port> ports)
		: Module(ports) { }

	void send(const u8 data);
};


class Gen1ModuleType : public ModuleType {
public:
	static std::vector<Port> gen1_ports;

	inline Gen1ModuleType() : ModuleType(GEN8, gen1_ports) { }

	inline Gen1Module* create() {
		return new Gen1Module(ports);
	}
};


/* ================
 * 8-bit generator
 */


class Gen8Module : public Module {
public:
	inline Gen8Module(const std::vector<Port> ports)
		: Module(ports) { }

	void send(const u8 data);
};


class Gen8ModuleType : public ModuleType {
/**
 * ports: 
 *	out: pin8
 */
public:
	static std::vector<Port> gen8_ports;
	inline Gen8ModuleType() : ModuleType(GEN8, gen8_ports) { }

	inline Gen8Module* create() {
		return new Gen8Module(ports);
	}
};



}

}


#endif /* VFEMU_MODULES_GEN_H */
