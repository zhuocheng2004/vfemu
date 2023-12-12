
#include <cstring>
#include <iostream>
#include <modules/pin.h>

namespace vfemu {

namespace pin {


bool Pin2pin::arePortsValid(Port* port1, Port* port2) {
	return !strcmp(port1->type, PIN8) && !strcmp(port2->type, PIN8);
}

Status Pin2pin::connect(Port* port1, Port* port2) {
	if (!port1 || !port2) {
		return Status::ERR_NULL;
	}
	if (!arePortsValid(port1, port2)) {
		return Status::ERR_INVALID;
	}

	port1->connector = new Pin2pinConnector(port2);
	port2->connector = new Pin2pinConnector(port1);
	return Status::SUCCESS;
}

Status Pin2pin::disconnect(Port* port1, Port* port2) {
	if (!arePortsValid(port1, port2)) {
		return Status::ERR_INVALID;
	}
	
	if (port1) {
		delete port1->connector;
	}
	if (port2) {
		delete port2->connector;
	}

	if (!port1 || !port2) {
		return Status::ERR_NULL;
	}
	return Status::SUCCESS;
}

} // namespace pin

} // namespace vfemu

