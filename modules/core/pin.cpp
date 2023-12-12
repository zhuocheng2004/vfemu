
#include <cstring>
#include <modules/pin.h>

namespace vfemu {

namespace pin {


bool Pin2pin::arePortsValid(Port* port1, Port* port2) {
	return !strcmp(port1->type, PIN8) && !strcmp(port2->type, PIN8);
}

Status Pin2pin::connect(Port* port1, Port* port2) {
	if (!arePortsValid(port1, port2)) {
		return Status::ERR_INVALID;
	}
	port1->send = port2->receive;
	port1->dest = port2->module;
	port2->send = port1->receive;
	port2->dest = port1->module;
	return Status::SUCCESS;
}

Status Pin2pin::disconnect(Port* port1, Port* port2) {
	if (!arePortsValid(port1, port2)) {
		return Status::ERR_INVALID;
	}
	port1->send = nullptr;
	port1->dest = nullptr;
	port2->send = nullptr;
	port2->dest = nullptr;
	return Status::SUCCESS;
}

} // namespace pin

} // namespace vfemu

