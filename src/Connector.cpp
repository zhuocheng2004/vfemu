
#include <list>
#include <vfemu/Connector.h>

namespace vfemu {

/**
 * list of registered connectors
 */
static std::list<VFEMUConnector*> connectors;


VFEMUConnector* getConnector(const char* name) {
	for (auto connector : connectors) {
		if (connector->name == name) {
			return connector;
		}
	}
	return nullptr;
}

VFEMUStatus registerConnector(VFEMUConnector* connector) {
	for (auto _connector : connectors) {
		if (_connector->name == connector->name) {
			return ERR_EXIST;
		}
	}
	connectors.push_front(connector);
	return SUCCESS;
}

VFEMUStatus unregisterConnector(VFEMUConnector* connector) {
	connectors.remove(connector);
	return SUCCESS;
}


VFEMUStatus connectPort(const char* connectorName, VFEMUPort* port1, VFEMUPort* port2) {
	auto connector = getConnector(connectorName);
	if (!connector) {
		return ERR_NONEXIST;
	}
	if (!connector->connect) {
		return ERR_NULL;
	}
	return connector->connect(port1, port2);
}

VFEMUStatus disconnectPort(const char* connectorName, VFEMUPort* port1, VFEMUPort* port2) {
	auto connector = getConnector(connectorName);
	if (!connector) {
		return ERR_NONEXIST;
	}
	if (!connector->disconnect) {
		return ERR_NULL;
	}
	return connector->disconnect(port1, port2);
}

}
