
#include <list>
#include <vfemu/Connector.h>

namespace vfemu {

/**
 * list of registered port types
 */
static std::list<VFEMUPortType*> portTypes;


VFEMUPortType* getPortType(const char* name) {
	for (auto portType : portTypes) {
		if (portType->name == name) {
			return portType;
		}
	}
	return nullptr;
}


VFEMUStatus registerPortType(VFEMUPortType* portType) {
	for (auto _portType: portTypes) {
		if (_portType->name == portType->name) {
			return ERR_EXIST;
		}
	}
	portTypes.push_front(portType);
	return SUCCESS;
}

VFEMUStatus unregisterPortType(VFEMUPortType* portType) {
	portTypes.remove(portType);
	return SUCCESS;
}

}
