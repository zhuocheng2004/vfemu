
#ifndef VFEMU_CONNECTOR_H
#define VFEMU_CONNECTOR_H


#include <vfemu/types.h>
#include <vfemu/Registry.h>
#include <vfemu/Port.h>


namespace vfemu {


class Connector {
public:
	/**
	 * This method needs to be set to NULL 
	 * during init. It will be filled during the
	 * matching process.
	 */
	inline virtual Status send(void* data) {
		return Status::SUCCESS;
	}

	inline virtual Status send(unsigned long data) {
		return send((void*) data);
	}
};


class Module;


/**
 * A connector defines how two ports should be connected
 */
class ConnectorType {
public:
	/**
	 * name of the connector
	 */
	const char*		name;

	static Registry<ConnectorType>	registry;

	/**
	 * method to [dis]connect two ports
	 */
	virtual Status connect(Port* port1, Port* port2);
	virtual Status disconnect(Port* port1, Port* port2);

	virtual Status connect(Module* module1, const char* id1, Module* module2, const char* id2);
	virtual Status disconnect(Module* module1, const char* id1, Module* module2, const char* id2);
};


} // namespace vfemu


#endif /* VFEMU_CONNECTOR_H */
