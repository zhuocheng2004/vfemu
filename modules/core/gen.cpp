
#include <cstdio>
#include <modules/gen.h>


namespace vfemu {

namespace gen {


std::vector<Port> Gen1ModuleType::gen1_ports = {
	Port("out", "pin1")
};

void Gen1Module::send(const u8 data) {
	Port port = ports[0];
	if (port.connector) {
		port.connector->send((void *) (unsigned long) data);
	}
}


std::vector<Port> Gen8ModuleType::gen8_ports = {
	Port("out", "pin8")
};

void Gen8Module::send(const u8 data) {
	Port port = ports[0];
	if (port.connector) {
		port.connector->send((void *) (unsigned long) data);
	}
}


}

}
