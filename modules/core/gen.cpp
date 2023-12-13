
#include <cstdio>
#include <vfemu/Connector.h>
#include <modules/gen.h>


namespace vfemu {

namespace gen {


void Gen1Module::send(const u8 data) {
	Port port = ports[0];
	if (port.connector) {
		port.connector->send((void *) (unsigned long) data);
	}
}

void Gen8Module::send(const u8 data) {
	Port port = ports[0];
	if (port.connector) {
		port.connector->send((void *) (unsigned long) data);
	}
}


}

}
