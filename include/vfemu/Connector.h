
#ifndef VFEMU_CONNECTOR_H
#define VFEMU_CONNECTOR_H


#include <vfemu/types.h>
#include <vfemu/Registry.h>
#include <vfemu/Port.h>


namespace vfemu {

/**
 * A connector defines how two ports should be connected
 */
class Connector {
public:
	/**
	 * name of the connector
	 */
	const char*		name;

	static Registry<Connector>	registry;

	/**
	 * method to connect two ports
	 */
	static Status		connect(Port* port1, Port* port2);

	/**
	 * method to disconnect two ports
	 */
	static Status		disconnect(Port* port1, Port* port2);

	static Status		connectPort(const char* connectorName, Port* port1, Port* port2);
	static Status		disconnectPort(const char* connectorName, Port* port1, Port* port2);
};


} // namespace vfemu


#endif /* VFEMU_CONNECTOR_H */
