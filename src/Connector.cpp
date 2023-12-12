
#include <list>
#include <vfemu/Connector.h>

namespace vfemu {

/**
 * list of registered connectors
 */
static std::list<Connector*> connectors;


Registry<Connector> Connector::registry;


Status Connector::connect(Port* port1, Port* port2) {
	return Status::SUCCESS;
}

Status Connector::disconnect(Port* port1, Port* port2) {
	return Status::SUCCESS;
}


Status Connector::connectPort(const char* connectorName, Port* port1, Port* port2) {
	auto connector = Connector::registry.get(connectorName);
	if (!connector) {
		return Status::ERR_NONEXIST;
	}
	if (!connector->connect) {
		return Status::ERR_NULL;
	}
	return connector->connect(port1, port2);
}

Status Connector::disconnectPort(const char* connectorName, Port* port1, Port* port2) {
	auto connector = Connector::registry.get(connectorName);
	if (!connector) {
		return Status::ERR_NONEXIST;
	}
	if (!connector->disconnect) {
		return Status::ERR_NULL;
	}
	return connector->disconnect(port1, port2);
}


}
