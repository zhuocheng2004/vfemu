
#include <vfemu/constants.h>
#include <modules/gen.h>


namespace vfemu {

namespace gen {

const std::vector<Port> gen8_ports = {
	Port("out", "pin8")
};

void U8Controller::send(const u8 data) {
	Port port = module->getPort(0);
	if (port.connector) {
		port.connector->send((void *) (unsigned long) data);
	}
}


}

}