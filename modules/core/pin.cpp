
#include <cstring>
#include <modules/pin.h>

using namespace vfemu;

VFEMUPortType pin8 = {
	.name =		NAME_PIN8
};


static bool arePortsValid(VFEMUPort* port1, VFEMUPort* port2) {
	return !strcmp(port1->type, NAME_PIN8) && !strcmp(port2->type, NAME_PIN8);
}

static VFEMUStatus pin2pin_connect(VFEMUPort* port1, VFEMUPort* port2) {
	if (!arePortsValid(port1, port2)) {
		return ERR_INVALID;
	}
	port1->send = port2->receive;
	port1->dest = port2->module;
	port2->send = port1->receive;
	port2->dest = port1->module;
	return SUCCESS;
}

static VFEMUStatus pin2pin_disconnect(VFEMUPort* port1, VFEMUPort* port2) {
	if (!arePortsValid(port1, port2)) {
		return ERR_INVALID;
	}
	port1->send = nullptr;
	port1->dest = nullptr;
	port2->send = nullptr;
	port2->dest = nullptr;
	return SUCCESS;
}


VFEMUConnector pin2pin = {
	.name =		"pin2pin",
	.connect =	pin2pin_connect,
	.disconnect =	pin2pin_disconnect,
};
