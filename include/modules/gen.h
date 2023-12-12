
#ifndef VFEMU_MODULES_GEN_H
#define VFEMU_MODULES_GEN_H

#include <vfemu/Module.h>
#include <modules/pin.h>

namespace vfemu {

namespace gen {


#define GEN8 "gen8"


class Gen8Module;

class U8Controller {
private:
	Gen8Module* module;
public:
	inline U8Controller(Gen8Module* module) {
		this->module = module;
	}

	void send(const u8 data);
};


extern const std::vector<Port> gen8_ports;


class Gen8Module : public Module {
public:
	inline Gen8Module(const std::vector<Port> ports)
		: Module(ports) { }

	inline Status init(U8Controller** controller) {
		*controller = new U8Controller(this);
		return Status::SUCCESS;
	}
};


class Gen8ModuleType : public ModuleType {
/**
 * ports: 
 *	out: pin8
 */
public:
	inline Gen8ModuleType() : ModuleType(GEN8, 1, gen8_ports) { }

	inline Gen8Module* create() {
		return new Gen8Module(ports);
	}
};



}

}


#endif /* VFEMU_MODULES_GEN_H */
